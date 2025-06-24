// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "application.h"

#include "resource.h"
#include "util.h"


// Cf. https://github.com/microsoftarchive/msdn-code-gallery-microsoft/blob/master/OneCodeTeam/UAC%20self-elevation%20(CppUACSelfElevation)/%5BC++%5D-UAC%20self-elevation%20(CppUACSelfElevation)/C++/CppUACSelfElevation/CppUACSelfElevation.cpp
#if defined(_M_IX86)
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined(_M_X64)
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else /* defined(_M_IX86) */
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif /* defined(_M_IX86) */


/*
 * application::run
 */
int application::run(_In_ const int show_command) {
    ::InitCommonControls();
    //::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Make sure that our window class is registered.
    {
        WNDCLASSEXW wc;
        wc.cbSize = sizeof(wc);

        if (!::GetClassInfoExW(this->_instance, window_class, &wc)) {
            ::ZeroMemory(&wc, sizeof(wc));
            wc.cbSize = sizeof(wc);

            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = application::wnd_proc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = this->_instance;
            wc.hIcon = ::LoadIconW(this->_instance, MAKEINTRESOURCEW(IDI_OXRSWITCH));
            wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
            wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
            //wc.lpszMenuName = MAKEINTRESOURCEW(IDC_OXRSWITCH);
            wc.lpszClassName = window_class;
            wc.hIconSm = ::LoadIconW(this->_instance, MAKEINTRESOURCEW(IDI_OXRSWITCH));

            THROW_LAST_ERROR_IF(!::RegisterClassExW(&wc));
        }
    }

    // Create the window.
    {
        auto title = ::load_wstring(this->_instance, IDS_APP_TITLE);
        auto exstyle = WS_EX_DLGMODALFRAME;
        auto style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
        this->_wnd.reset(::CreateWindowExW(exstyle,
            window_class,
            title.c_str(),
            style,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            nullptr,
            nullptr,
            this->_instance,
            this));
        THROW_LAST_ERROR_IF(!this->_wnd);

        // Instantiate the templated dialog as child window.
        this->_dlg.reset(CreateDialogParamW(
            this->_instance,
            MAKEINTRESOURCEW(IDD_SELECTDIALOG),
            this->_wnd.get(),
            dlg_proc,
            0));
        THROW_LAST_ERROR_IF(!this->_dlg);
        ::SetWindowLongPtrW(this->_dlg.get(),
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(this));

        if (!::is_elevated()) {
            HWND btn_permissions = ::GetDlgItem(this->_dlg.get(),
                IDC_PERMISSIONS);
            THROW_LAST_ERROR_IF(!btn_permissions);
            Button_SetElevationRequiredState(btn_permissions, TRUE);
        }

        // Populate the runtimes.
        HWND cb = ::GetDlgItem(this->_dlg.get(), IDC_COMBO_RUNTIMES);
        THROW_LAST_ERROR_IF(!cb);

        for (auto& r : this->_manager) {
            ::SendMessageW(cb,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(r.name().c_str()));
        }

        int selected = -1;
        try {
            this->_manager.active_runtime(&selected);
        } catch (...) { /* Just select nothing in this case. */ }
        ::SendMessageW(cb, CB_SETCURSEL, selected, 0);

        ::ShowWindow(this->_dlg.get(), SW_SHOW);

        // Ajust the overall window to the size of the dialog.
        RECT r;
        THROW_LAST_ERROR_IF(!::GetWindowRect(this->_dlg.get(), &r));
        THROW_LAST_ERROR_IF(!::AdjustWindowRect(&r, style, FALSE));
        const auto w = std::abs(r.right - r.left);
        const auto h = std::abs(r.bottom - r.top);
        THROW_LAST_ERROR_IF(!::SetWindowPos(this->_wnd.get(), NULL, 0, 0, w, h,
            SWP_NOMOVE));
    }


    // Make the window visible.
    ::ShowWindow(this->_wnd.get(), show_command);
    ::UpdateWindow(this->_wnd.get());

    // Enter the message loop.
    {
        MSG msg;

        while (::GetMessage(&msg, nullptr, 0, 0)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }
}


/*
 * application::add_ace
 */
int application::add_ace(_In_ wil::unique_hkey& key) {
    if (!key) {
        return ERROR_NOT_FOUND;
    }

    wil::unique_hlocal sd;
    PACL existing_acl = nullptr;
    RETURN_IF_WIN32_ERROR(::GetSecurityInfo(key.get(),
        SE_KERNEL_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &existing_acl,
        nullptr,
        sd.put()));

    wil::unique_sid auth_users;
    RETURN_IF_WIN32_ERROR(authenticated_users(auth_users));

    EXPLICIT_ACCESS ea;
    ::ZeroMemory(&ea, sizeof(ea));
    ea.grfAccessPermissions = KEY_READ | KEY_QUERY_VALUE | KEY_SET_VALUE;
    ea.grfAccessMode = SET_ACCESS;// GRANT_ACCESS??
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea.Trustee.ptstrName = reinterpret_cast<LPWSTR>(auth_users.get());

    wil::unique_hlocal new_acl;
    RETURN_IF_WIN32_ERROR(::SetEntriesInAclW(1, &ea, existing_acl,
        reinterpret_cast<PACL *>(new_acl.put())));

    RETURN_IF_WIN32_ERROR(::SetSecurityInfo(key.get(),
        SE_KERNEL_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        static_cast<PACL>(new_acl.get()),
        nullptr));

    return ERROR_SUCCESS;
}


/*
 * application::about_proc
 */
LRESULT CALLBACK application::about_proc(
        _In_ const HWND dlg,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam) {
    UNREFERENCED_PARAMETER(lparam);

    switch (message) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDOK:
                case IDCANCEL:
                    ::EndDialog(dlg, LOWORD(wparam));
                    return TRUE;
            }
            break;
    }

    return FALSE;
}


/*
 * application::authenticated_users
 */
int application::authenticated_users(_Out_ wil::unique_sid& retval) {
    SID_IDENTIFIER_AUTHORITY nt_auth = SECURITY_NT_AUTHORITY;
    RETURN_LAST_ERROR_IF(!::AllocateAndInitializeSid(&nt_auth, 1,
        SECURITY_AUTHENTICATED_USER_RID,
        0, 0, 0, 0, 0, 0, 0,
        retval.put()));
    return ERROR_SUCCESS;
}


/*
 * application::dlg_proc
 */
LRESULT CALLBACK application::dlg_proc(
        _In_ const HWND dlg,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam) {
    UNREFERENCED_PARAMETER(lparam);
    auto that = reinterpret_cast<application *>(
        ::GetWindowLongPtrW(dlg, GWLP_USERDATA));

    switch (message) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDCLOSE:
                    ::PostQuitMessage(0);
                    return TRUE;

                case IDC_COMBO_RUNTIMES:
                    if (HIWORD(wparam) == CBN_SELCHANGE) {
                        // If the selection in the combobox changes, notify the
                        // runtime manager to update the active runtime.
                        try {
                            that->_manager.active_runtime(static_cast<int>(
                                ::SendMessageW(reinterpret_cast<HWND>(lparam),
                                    CB_GETCURSEL, 0, 0)));
                        } catch (std::exception& ex) {
                            ::MessageBoxA(that->_wnd.get(), ex.what(), nullptr,
                                MB_OK | MB_ICONERROR);
                        }
                    }
                    return TRUE;

                case IDC_PERMISSIONS:
                    try {
                        if (::is_elevated()) {
                            THROW_IF_WIN32_ERROR(fix_acls());
                        } else {
                            std::wstring cmd = ::get_module_path(NULL);
                            SHELLEXECUTEINFOW execute;
                            DWORD exit_code = 0;

                            ::ZeroMemory(&execute, sizeof(execute));
                            execute.cbSize = sizeof(execute);
                            execute.lpVerb = L"runas";
                            execute.lpFile = cmd.c_str();
                            execute.lpParameters = L"/fixacls";
                            execute.nShow = SW_HIDE;
                            execute.fMask = SEE_MASK_NOCLOSEPROCESS;

                            THROW_LAST_ERROR_IF(!::ShellExecuteExW(&execute));
                            auto guard_proc = wil::scope_exit([&execute]() {
                                ::CloseHandle(execute.hProcess);
                            });
                            THROW_IF_WIN32_ERROR(::WaitForSingleObject(
                                execute.hProcess, 5000));
                            THROW_LAST_ERROR_IF(!::GetExitCodeProcess(
                                execute.hProcess, &exit_code));
                            THROW_IF_WIN32_ERROR(exit_code);

                            // Recreate the manager with proper access.
                            that->_manager = runtime_manager();
                        }
                    } catch (std::exception& ex) {
                        ::MessageBoxA(that->_wnd.get(), ex.what(), nullptr,
                            MB_OK | MB_ICONERROR);
                    }
                    return TRUE;
            }
            break;
    }

    return FALSE;
}


/*
 * application::is_ace
 */
bool application::is_ace(_In_ const PACE_HEADER ace,
        _In_ const wil::unique_sid& user) {
    if ((ace == nullptr) || (user == nullptr)) {
        return false;
    }

    if (ace->AceType != ACCESS_ALLOWED_ACE_TYPE) {
        return false;
    }

    auto entry = reinterpret_cast<PACCESS_ALLOWED_ACE>(ace);
    auto sid = reinterpret_cast<PSID>(std::addressof(entry->SidStart));

    if (!::EqualSid(sid, user.get())) {
        return false;
    }

    return (entry->Mask == (KEY_READ | KEY_QUERY_VALUE | KEY_SET_VALUE));
}


/*
 * application::remove_ace
 */
int application::remove_ace(_In_ wil::unique_hkey& key) {
    if (!key) {
        return ERROR_NOT_FOUND;
    }

    wil::unique_hlocal sd;
    PACL old_acl = nullptr;
    RETURN_IF_WIN32_ERROR(::GetSecurityInfo(key.get(),
        SE_KERNEL_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &old_acl,
        nullptr,
        sd.put()));

    wil::unique_sid auth_users;
    RETURN_IF_WIN32_ERROR(authenticated_users(auth_users));

    wil::unique_hlocal new_acl(::LocalAlloc(LPTR, old_acl->AclSize));
    if (!new_acl) {
        return ERROR_OUTOFMEMORY;
    }

    RETURN_LAST_ERROR_IF(!::InitializeAcl(
        reinterpret_cast<PACL>(new_acl.get()),
        old_acl->AclSize,
        ACL_REVISION));

    for (WORD i = 0, j = 0; i < old_acl->AceCount; ++i) {
        PACE_HEADER ace;
        RETURN_LAST_ERROR_IF(!::GetAce(old_acl, i,
            reinterpret_cast<void **>(&ace)));

        if (!is_ace(ace, auth_users)) {
            RETURN_LAST_ERROR_IF(!::AddAce(
                reinterpret_cast<PACL>(new_acl.get()),
                ACL_REVISION,
                j++,
                ace,
                ace->AceSize));
        }
    }

    RETURN_IF_WIN32_ERROR(::SetSecurityInfo(key.get(),
        SE_KERNEL_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        static_cast<PACL>(new_acl.get()),
        nullptr));

    return ERROR_SUCCESS;
}


/*
 * application::wnd_proc
 */
LRESULT CALLBACK application::wnd_proc(
        _In_ const HWND wnd,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam) {
    auto that = reinterpret_cast<application *>(
        ::GetWindowLongPtrW(wnd, GWLP_USERDATA));

    switch (message) {
        case WM_CREATE: {
            auto cs = reinterpret_cast<CREATESTRUCTW *>(lparam);
            return (::SetWindowLongPtrW(wnd, GWLP_USERDATA,
                    reinterpret_cast<LONG_PTR>(cs->lpCreateParams)) == 0)
                ? ::GetLastError()
                : 0;
            }

        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;

        default:
            return ::DefWindowProcW(wnd, message, wparam, lparam);
    }
}
