// <copyright file="oxrswitch.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"

#include "application.h"
#include "resource.h"


/// <summary>
/// Entry point of the application.
/// </summary>
/// <param name="instance"></param>
/// <param name="previous_instance"></param>
/// <param name="command_line"></param>
/// <param name="show_command"></param>
/// <returns></returns>
int APIENTRY wWinMain(_In_ const HINSTANCE instance,
        _In_opt_ const HINSTANCE previous_instance,
        _In_ LPWSTR command_line,
        _In_ const int show_command) {
    UNREFERENCED_PARAMETER(previous_instance);
    UNREFERENCED_PARAMETER(command_line);

    try {
        if (equals(command_line, L"/fixacls", false)) {
            return application::fix_acls();

        } else if (equals(command_line, L"/unfixacls", false)) {
            return application::unfix_acls();

        } else {
            application app(instance);
            return app.run(show_command);
        }
    } catch (std::exception& ex) {
        ::OutputDebugStringA(ex.what());
        ::MessageBoxA(NULL, ex.what(), nullptr, MB_OK | MB_ICONERROR);
        return -1;
    } catch (...) {
        auto msg = ::load_wstring(instance, IDS_ERROR_UNEXPECTED);
        ::MessageBoxW(NULL, msg.c_str(), nullptr, MB_OK | MB_ICONERROR);
        return -2;
    }
}
