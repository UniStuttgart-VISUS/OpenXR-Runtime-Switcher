﻿// <copyright file="runtime_manager.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_RUNTIME_MANAGER_H)
#define _OXRSWITCH_RUNTIME_MANAGER_H
#pragma once

#include "path_compare.h"
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
    /// Opens the OpenXR keys for the native and possibly the WOW64 system.
    /// </summary>
    /// <returns>The two keys where the active runtimes are stored. One or both
    /// keys could be invalid if they do not exist as the method will not throw.
    /// </returns>
    static std::pair<wil::unique_hkey, wil::unique_hkey> open_keys(void);

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    inline runtime_manager(void)
            : _key(get_openxr_key(openxr_key, true)),
            _wow_key(get_openxr_key(wow_key, true)) {
        this->load_runtimes();
    }

    /// <summary>
    /// Answer the currently active runtime according to the registry.
    /// </summary>
    /// <param name="index">If non <see langword="nullptr" />, receives the
    /// zero-based index of the selected runtime among the ones known to the
    /// manager. This parameter defaults to <see langword="nullptr" />.</param>
    /// <returns>The active runtime.</returns>
    const runtime& active_runtime(_Out_opt_ int *index = nullptr) const;

    /// <summary>
    /// Sets a new active runtime.
    /// </summary>
    /// <param name="runtime">The runtime to activate.</param>
    void active_runtime(_In_ const runtime& runtime);

    /// <summary>
    /// Sets a new active runtime.
    /// </summary>
    /// <param name="index">The zero-based index of the runtime to be
    /// activated.</param>
    void active_runtime(_In_ const std::size_t index);

private:

    /// <summary>
    /// Gets the available OpenXR runtimes registered in the registry.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="key"></param>
    /// <param name="oit"></param>
    template<class TIterator>
    static void get_available_runtimes(_In_ const wil::unique_hkey& key,
        _In_ TIterator oit);

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
    /// Gets the registry key of the latest OpenXR installation.
    /// </summary>
    /// <param name="path">The path to the OpenXR key, which must be one of
    /// <see cref="openxr_key" /> or <paramref name="wow_key" /></param>
    /// <param name="lenient">Controls whether the method throws on error
    /// (<see langword="false" />) or returns an invalid key
    /// (<see langword="true" />).</param>
    /// <returns></returns>
    static wil::unique_hkey get_openxr_key(_In_z_ const wchar_t *path,
        _In_ const bool lenient);

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
    /// Merges the runtimes and their potential WOW64 counterparts into a
    /// <see cref="runtime" /> instance.
    /// </summary>
    /// <typeparam name="TIterator">An iterator over the paths to the runtime
    /// JSON files and their potential WOW64 counterparts.</typeparam>
    /// <typeparam name="TOutIterator">An output iterator for
    /// <see cref="runtime" />s.</typeparam>
    /// <param name="path_begin"></param>
    /// <param name="path_end"></param>
    /// <param name="wow_begin"></param>
    /// <param name="path_begin"></param>
    /// <param name="wow_end"></param>
    /// <param name="oit"></param>
    template<class TIterator, class TOutIterator> static void make_runtimes(
        _In_ const TIterator path_begin, _In_ const TIterator path_end,
        _In_ const TIterator wow_begin, _In_ const TIterator wow_end,
        _In_ TOutIterator oit);

    /// <summary>
    /// Read at exactly <paramref name="cnt" /> bytes from
    /// <paramref name="handle" />.
    /// </summary>
    /// <param name="handle"></param>
    /// <param name="data"></param>
    /// <param name="cnt"></param>
    static void read(_In_ wil::unique_hfile& handle,
        _Out_writes_bytes_(cnt) void *data,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Write all <paramref name="cnt" /> bytes to <paramref name="handle" />.
    /// </summary>
    /// <param name="handle"></param>
    /// <param name="data"></param>
    /// <param name="cnt"></param>
    static void write(_In_ wil::unique_hfile& handle,
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt);

    /// <summary>
    /// The name of the registry value that stores the active runtime.
    /// </summary>
    static constexpr const wchar_t *const active_runtime_value
        = L"ActiveRuntime";

    /// <summary>
    /// The path in the registry where the OpenXR stuff is stored.
    /// </summary>
    static constexpr const wchar_t *const openxr_key = L"SOFTWARE\\Khronos\\"
        L"OpenXR";

    /// <summary>
    /// The name of the named pipe we use to communicate with the tool.
    /// </summary>
    static constexpr const wchar_t *const pipe_name = L"\\\\.\\pipe\\oxrswitch";

    /// <summary>
    /// The path in the registry to the 32-bit stuff of OpenXR.
    /// </summary>
    static constexpr const wchar_t *const wow_key = L"SOFTWARE\\WOW6432Node\\"
        "Khronos\\OpenXR";

    /// <summary>
    /// Loads all OpenXR runtimes we can find.
    /// </summary>
    void load_runtimes(void);

    wil::unique_hkey _key;
    std::vector<runtime> _runtimes;
    wil::unique_hkey _wow_key;

public:

    /// <summary>
    /// Gets an iterator for the begin of the range of available runtimes.
    /// </summary>
    /// <returns></returns>
    inline auto begin(void) const -> decltype(this->_runtimes.begin()) {
        return this->_runtimes.begin();
    }

    /// <summary>
    /// Gets an iterator for the end of the range of available runtimes.
    /// </summary>
    /// <returns></returns>
    inline auto end(void) const -> decltype(this->_runtimes.end()) {
        return this->_runtimes.end();
    }
};

#include "runtime_manager.inl"

#endif /* !defined(_OXRSWITCH_RUNTIME_MANAGER_H) */
