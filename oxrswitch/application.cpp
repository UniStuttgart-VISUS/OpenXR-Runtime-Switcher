// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "application.h"

#include "resource.h"


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
            wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
            wc.lpszMenuName = MAKEINTRESOURCEW(IDC_OXRSWITCH);
            wc.lpszClassName = window_class;
            wc.hIconSm = ::LoadIconW(this->_instance, MAKEINTRESOURCEW(IDI_SMALL));

            THROW_LAST_ERROR_IF(!::RegisterClassExW(&wc));
        }
    }

    // Create the window.
    {
        auto title = ::load_wstring(this->_instance, IDS_APP_TITLE);
        this->_wnd.reset(::CreateWindowW(window_class,
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            nullptr,
            nullptr,
            this->_instance,
            nullptr));
        THROW_LAST_ERROR_IF(!this->_wnd);
    }

    this->_dlg.reset(CreateDialogParamW(
        this->_instance,
        MAKEINTRESOURCEW(IDD_SELECTDIALOG),
        this->_wnd.get(),
        wnd_proc,
        0));
    THROW_LAST_ERROR_IF(!this->_dlg);
    ::ShowWindow(this->_dlg.get(), SW_SHOW);

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
 * application::wnd_proc
 */
LRESULT CALLBACK application::wnd_proc(
        _In_ const HWND wnd,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam) {
    switch (message) {
    //    case WM_COMMAND:
    //    {
    //        int wmId = LOWORD(wParam);
    //        // Parse the menu selections:
    //        switch (wmId) {
    //            case IDM_ABOUT:
    //                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
    //                break;
    //            case IDM_EXIT:
    //                DestroyWindow(hWnd);
    //                break;
    //            default:
    //                return DefWindowProc(hWnd, message, wParam, lParam);
    //        }
    //    }
    //    break;
    //    case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hWnd, &ps);
    //        // TODO: Add any drawing code that uses hdc here...
    //        EndPaint(hWnd, &ps);
    //    }
    //    break;

        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;

        default:
            return ::DefWindowProcW(wnd, message, wparam, lparam);
    }
}
