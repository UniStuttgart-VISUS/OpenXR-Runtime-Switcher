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
    auto software = get_software();
    auto openxr = get_openxr_software(software);
}


/*
 * runtime_manager::get_openxr_software
 */
std::vector<std::wstring> runtime_manager::get_openxr_software(
        _In_ const std::vector<wil::unique_hkey>& software) {
    std::vector<std::wstring> retval;

    for (auto& s : software) {
        for (auto& r : runtime_info::runtimes) {
            std::wstring path;
            if (is_match(s, r, path)) {
                retval.push_back(std::move(path));
            }
        }
    }

    return retval;
}


/*
 * runtime_manager::get_software
 */
std::vector<wil::unique_hkey> runtime_manager::get_software(void) {
    std::vector<wil::unique_hkey> retval;

    // Native software.
    {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
        auto cnt = wil::reg::get_child_key_count(key.get());
        retval.reserve(retval.size() + cnt);

        for (auto it = wil::reg::key_iterator(key.get()),
                end = wil::reg::key_iterator(); it != end; ++it) {
            retval.emplace_back(wil::reg::open_unique_key(key.get(),
                it->name.c_str()));
        }
    }

    // 32-bit software on 64-bit systems.
    try {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE\\"
            L"WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
        auto cnt = wil::reg::get_child_key_count(key.get());
        retval.reserve(retval.size() + cnt);

        for (auto it = wil::reg::key_iterator(key.get()),
            end = wil::reg::key_iterator(); it != end; ++it) {
            retval.emplace_back(wil::reg::open_unique_key(key.get(),
                it->name.c_str()));
        }
    } catch(...) { /* This is not fatal. */ }

    return retval;
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

        if (std::regex_match(publisher, info.vendor())
                && std::regex_match(name, info.software())) {
            return true;
        }
    } catch (...) { /* Not an acceptable candidate. */ }

    return false;
}
