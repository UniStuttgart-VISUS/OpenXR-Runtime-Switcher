// <copyright file="runtime_manager.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_RUNTIME_MANAGER_H)
#define _OXRSWITCH_RUNTIME_MANAGER_H
#pragma once

#include "runtime_info.h"


/// <summary>
/// The runtime manager class detects known OpenXR runtimes on the system and
/// changes the active runtime.
/// </summary>
class runtime_manager {

public:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    runtime_manager(void);

private:

    /// <summary>
    /// Filters the given uninstall keys for known OpenXR runtimes and answers
    /// their installation paths.
    /// </summary>
    /// <param name="software"></param>
    /// <returns></returns>
    static std::vector<std::wstring> get_openxr_software(
        _In_ const std::vector<wil::unique_hkey>& software);

    /// <summary>
    /// Enumerates all installed software in the uninstall database of the
    /// registry.
    /// </summary>
    /// <returns>A vector of the keys holding the uninstall information.
    /// </returns>
    static std::vector<wil::unique_hkey> get_software(void);

    /// <summary>
    /// Answer whether the given software key is the given OpenXR runtime, and
    /// if so, return the installation path.
    /// </summary>
    /// <param name="key"></param>
    /// <param name="info"></param>
    /// <param name="path"></param>
    /// <returns></returns>
    _Success_(return) static bool is_match(_In_ const wil::unique_hkey& key,
        _In_ const runtime_info& info,
        _Out_ std::wstring& path);

};

#endif /* !defined(_OXRSWITCH_RUNTIME_MANAGER_H) */
