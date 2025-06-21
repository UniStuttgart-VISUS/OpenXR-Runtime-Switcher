// <copyright file="runtime_manager.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_APPLICATION_H)
#define _OXRSWITCH_APPLICATION_H
#pragma once

#include "runtime_manager.h"


/// <summary>
/// Manages the application itself including the message pump.
/// </summary>
class application final {

public:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="instance">The instance handle of the application.</param>
    inline application(_In_ const HINSTANCE instance) noexcept
        : _instance(instance) { }

    application(const application&) = delete;

    /// <summary>
    /// Runs the message loop.
    /// </summary>
    /// <param name="show_command"></param>
    /// <returns>The exit code of the application.</returns>
    int run(_In_ const int show_command);

    application operator =(const application&) = delete;

private:

    static constexpr const wchar_t *const window_class = L"OXRSWITCHWND";

    static LRESULT CALLBACK about_proc(_In_ const HWND dlg,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam);

    static LRESULT CALLBACK dlg_proc(_In_ const HWND dlg,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam);

    static LRESULT CALLBACK wnd_proc(_In_ const HWND wnd,
        _In_ const UINT message,
        _In_ const WPARAM wparam,
        _In_ const LPARAM lparam);

    wil::unique_hwnd _dlg;
    HINSTANCE _instance;
    runtime_manager _manager;
    wil::unique_hwnd _wnd;
};

#endif /* !defined(_OXRSWITCH_APPLICATION_H) */
