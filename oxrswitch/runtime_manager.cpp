// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime_manager.h"


/*
 * runtime_manager::runtime_manager
 */
runtime_manager::runtime_manager(void) {
    std::vector<std::wstring> paths;
    get_uninstall_paths(paths);
    get_software_paths(paths);

    this->_runtimes.reserve(paths.size());
    for (auto& p : paths) {
        this->_runtimes.emplace_back(p);
    }

    this->_runtimes.emplace_back(L"\\\\virelai\\c$\\Program Files\\Oculus\\Support\\oculus-runtime\\oculus_openxr_64.json");
}


/*
 * runtime_manager::get_software_paths
 */
void runtime_manager::get_software_paths(
        _Inout_ std::vector<std::wstring>& paths) {

    // Native software.
    {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE");
        get_software_paths(key, paths);
    }

    // 32-bit software on 64-bit systems.
    try {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE\\"
            L"WOW6432Node");
        get_software_paths(key, paths);
    } catch (...) { /* This is not fatal. */ }
}


/*
 * runtime_manager::get_software_paths
 */
void runtime_manager::get_software_paths(_In_ const wil::unique_hkey& key,
        _Inout_ std::vector<std::wstring>& paths) {
    assert(key);

    for (auto it = wil::reg::key_iterator(key.get()),
            end = wil::reg::key_iterator(); it != end; ++it) {
        // Get the "vendor" key below SOFTWARE/WOW6432Node.
        auto v = wil::reg::open_unique_key(key.get(), it->name.c_str());

        for (auto jt = wil::reg::key_iterator(v.get()); jt != end; ++jt) {
            // 'jt' goes over the per-vendor software entries in the vendor key.
            // If the name of this key in combination with the vendor name in
            // 'it' match any of the known runtimes, try to derive the
            // installation location from it.

            for (auto& r : runtime_info::runtimes) {
                if (r.is_match(it->name, jt->name)) {
                    std::wstring path;
                    auto s = wil::reg::open_unique_key(v.get(),
                        jt->name.c_str());
                    if (r.try_get_installation_path(s, path)) {
                        paths.push_back(std::move(path));
                    }
                }
            }
        }
    }
}


/*
 * runtime_manager::get_uninstall_paths
 */
void runtime_manager::get_uninstall_paths(
        _Inout_ std::vector<std::wstring>& paths) {

    // Native software.

    {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
        get_uninstall_paths(key, paths);
    }

    // 32-bit software on 64-bit systems.
    try {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE\\"
            L"WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
        get_uninstall_paths(key, paths);
    } catch(...) { /* This is not fatal. */ }
}


/*
 * runtime_manager::get_uninstall_paths
 */
void runtime_manager::get_uninstall_paths(_In_ const wil::unique_hkey& key,
        _Inout_ std::vector<std::wstring>& paths) {
    assert(key);
    auto cnt = wil::reg::get_child_key_count(key.get());

    for (auto it = wil::reg::key_iterator(key.get()),
            end = wil::reg::key_iterator(); it != end; ++it) {
        auto k = wil::reg::open_unique_key(key.get(), it->name.c_str());

        for (auto& r : runtime_info::runtimes) {
            std::wstring path;
            if (is_match(k, r, path)) {
                paths.push_back(std::move(path));
            }
        }
    }
}


/*
 * runtime_manager::is_match
 */
_Success_(return) bool runtime_manager::is_match(
        _In_ const wil::unique_hkey& key,
        _In_ const runtime_info& info,
        _Out_ std::wstring& path) {
    assert(key);

    try {
        auto name = wil::reg::get_value_string(key.get(), L"DisplayName");
        auto publisher = wil::reg::get_value_string(key.get(), L"Publisher");

        path = wil::reg::get_value_string(key.get(), L"InstallLocation");
        return info.is_match(publisher, name);
    } catch (...) {
        return false;
    }
}
