// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "application.h"

#include "resource.h"
#include "util.h"


/*
 * application::run
 */
int application::run(_In_ const int show_command) {
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
            wc.hIconSm = ::LoadIconW(this->_instance, MAKEINTRESOURCEW(IDI_SMALL));

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
            }
            break;
    }

    return FALSE;
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
