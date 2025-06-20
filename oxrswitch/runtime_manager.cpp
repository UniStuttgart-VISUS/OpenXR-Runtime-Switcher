// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime_manager.h"

#include "path_compare.h"


/*
 * runtime_manager::runtime_manager
 */
runtime_manager::runtime_manager(void) {
    std::set<std::wstring, path_compare> install_paths;
    get_uninstall_paths(std::inserter(install_paths, install_paths.begin()));
    get_software_paths(std::inserter(install_paths, install_paths.begin()));

    this->_runtimes.reserve(install_paths.size());
    for (auto& p : install_paths) {
        std::vector<std::wstring> candidates;
        get_json_files(p, std::back_inserter(candidates));

        for (auto& c : candidates) {
            try {
                this->_runtimes.push_back(runtime::from_file(c));
            } catch (...) { /* Candidate invalid. */ }
        }
    }

    this->_runtimes.push_back(runtime::from_file(L"\\\\virelai\\c$\\Program Files\\Oculus\\Support\\oculus-runtime\\oculus_openxr_64.json"));
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
