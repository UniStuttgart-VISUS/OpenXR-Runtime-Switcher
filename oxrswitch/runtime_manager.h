// <copyright file="runtime_manager.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_RUNTIME_MANAGER_H)
#define _OXRSWITCH_RUNTIME_MANAGER_H
#pragma once

#include "runtime.h"
#include "runtime_info.h"
#include "util.h"


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
    /// Gets the paths to all JSON files in <paramref name="folder" /> and
    /// writes them to <paramref name="oit" />.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="folder"></param>
    /// <param name="oit"></param>
    template<class TIterator>
    static void get_json_files(_In_ const std::wstring& folder,
        _In_ TIterator oit);

    /// <summary>
    /// Enumerates all vendor-specific software keys in the registry, both the
    /// standard ones as well as Wow64, and returns the installation paths
    /// derived from the ones matching known OpenXR runtimes.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="oit"></param>
    template<class TIterator>
    static void get_software_paths(_In_ TIterator oit);

    /// <summary>
    /// Enumerates all vendor/software paths in <paramref name="key" /> and
    /// returns the installation paths derived from the ones matching known
    /// OpenXR runtimes.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="key">Either the software key or the &quot;WOW6432Node&quot;
    /// in the software key.</param>
    /// <param name="oit"></param>
    template<class TIterator>
    static void get_software_paths(_In_ const wil::unique_hkey& key,
        _In_ TIterator oit);

    /// <summary>
    /// Enumerates all installed software in the uninstall database of the
    /// registry and returns the installation patsh from derived from the ones
    /// matching known OpenXR runtimes.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="oit"></param>
    template<class TIterator>
    static void get_uninstall_paths(_In_ TIterator oit);

    /// <summary>
    /// Enumerates the uninstall database identified by <paramref name="key" />
    /// and returns the installation patsh from derived from the ones matching
    /// known OpenXR runtimes.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="key"></param>
    /// <param name="oit"></param>
    template<class TIterator>
    static void get_uninstall_paths(_In_ const wil::unique_hkey& key,
        _In_ TIterator oit);

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

    /// <summary>
    /// The path in the registry where the OpenXR stuff is stored.
    /// </summary>
    static constexpr const wchar_t *const openxr_path = L"SOFTWARE\\Khronos\\"
        L"OpenXR";

    wil::unique_hkey _key;
    std::vector<runtime> _runtimes;
};

#include "runtime_manager.inl"

#endif /* !defined(_OXRSWITCH_RUNTIME_MANAGER_H) */
