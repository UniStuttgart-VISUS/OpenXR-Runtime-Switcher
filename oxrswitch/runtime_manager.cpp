// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime_manager.h"

#include "path_compare.h"
#include "resource.h"


/*
 * runtime_manager::runtime_manager
 */
runtime_manager::runtime_manager(void)
        : _key(get_openxr_key(openxr_key, true)),
        _wow_key(get_openxr_key(wow_key, true)) {
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

    //this->_runtimes.push_back(runtime::from_file(L"\\\\virelai\\c$\\Program Files\\Oculus\\Support\\oculus-runtime\\oculus_openxr_64.json"));
}


/*
 * runtime_manager::active_runtime
 */
const runtime& runtime_manager::active_runtime(_Out_opt_ int *index) const {
    auto rt = wil::reg::get_value_expanded_string(this->_key.get(),
        active_runtime_value);

    auto it = std::find_if(this->_runtimes.begin(),
        this->_runtimes.end(),
        [&rt](const runtime& r) { return equals(r.path(), rt, false); });

    if (it == this->_runtimes.end()) {
        throw std::runtime_error(::load_string(NULL, 0));
    }

    if (index != nullptr) {
        *index = static_cast<int>(std::distance(this->_runtimes.begin(), it));
    }

    return *it;
}


/*
 * runtime_manager::active_runtime
 */
void runtime_manager::active_runtime(_In_ const runtime& runtime) {
    wil::reg::set_value(this->_key.get(),
        active_runtime_value,
        runtime.path().c_str());

    if (this->_wow_key) {
        if (runtime.wow_path().empty()) {
            auto hr = HRESULT_FROM_WIN32(::RegDeleteValueW(this->_wow_key.get(),
                active_runtime_value));
            THROW_HR_IF(hr, FAILED(hr));

        } else {
            wil::reg::set_value(this->_wow_key.get(),
                active_runtime_value,
                runtime.wow_path().c_str());
        }

    }
}


/*
 * runtime_manager::get_openxr_key
 */
wil::unique_hkey runtime_manager::get_openxr_key(
        _In_z_ const wchar_t *path,
        _In_ const bool lenient) {
    assert(path != nullptr);
    try {
        auto retval = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, path);

        // If we the base key, get the subkey for the latest version.
        std::vector<std::wstring> versions;
        std::transform(wil::reg::key_iterator(retval.get()),
            wil::reg::key_iterator(),
            std::back_inserter(versions),
            [](const wil::reg::key_iterator::value_type& v) { return v.name; });

        // Sort such that the latest version is at the end. TODO: will break
        // at 10, I guess ...
        std::sort(versions.begin(), versions.end());
        retval = wil::reg::open_unique_key(retval.get(),
            versions.back().c_str(),
            wil::reg::key_access::readwrite);
        return retval;
    } catch (...) {
        if (lenient) {
            return wil::unique_hkey();
        } else {
            throw;
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
