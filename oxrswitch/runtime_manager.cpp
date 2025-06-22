// <copyright file="runtime_manager.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime_manager.h"

#include "resource.h"


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
            ::RegDeleteValueW(this->_wow_key.get(), active_runtime_value);
            // This may fail if no 32-bit runtime was installed in the first
            // place, which is fine. We therefore do not check this error.

        } else {
            wil::reg::set_value(this->_wow_key.get(),
                active_runtime_value,
                runtime.wow_path().c_str());
        }

    }
}


/*
 * runtime_manager::active_runtime
 */
void runtime_manager::active_runtime(_In_ const std::size_t index) {
    THROW_WIN32_IF(ERROR_INVALID_PARAMETER, index >= this->_runtimes.size());
    this->active_runtime(this->_runtimes[index]);
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


/*
 * runtime_manager::load_runtimes
 */
void runtime_manager::load_runtimes(void) {
    // As we expect that some runtimes will be discovered via multiple paths, we
    // collect anything in a set to avoid duplicates.
    std::set<runtime> runtimes;
    auto oit = std::inserter(runtimes, runtimes.begin());

    // First, get all known runtimes from the registry.
    {
        std::set<std::wstring, path_compare> paths;
        std::set<std::wstring, path_compare> wow_paths;
        get_available_runtimes(this->_key,
            std::inserter(paths, paths.begin()));
        get_available_runtimes(this->_wow_key,
            std::inserter(wow_paths, wow_paths.begin()));
        make_runtimes(paths.begin(), paths.end(),
            wow_paths.begin(), wow_paths.end(),
            oit);
    }

    // Second, add Windows Mixed Reality, which is not listed anywhere ...
    {
        auto p = ::expand_environment_variables(L"%SYSTEMROOT%\\System32\\"
            L"MixedRealityRuntime.json");
        auto w = ::expand_environment_variables(L"%SYSTEMROOT%\\SysWOW64\\"
            L"MixedRealityRuntime.json");
        if (::file_exists(p)) {
            if (::file_exists(w)) {
                *oit++ = runtime::from_file(p, w);
            } else {
                *oit++ = runtime::from_file(p);
            }
        }
    }

    // Third, look for runtimes in known installation paths.
    {
        std::set<std::wstring, path_compare> installs;
        get_uninstall_paths(std::inserter(installs, installs.begin()));
        get_software_paths(std::inserter(installs, installs.begin()));

        for (auto& p : installs) {
            std::set<runtime> candidates;
            std::vector<std::wstring> files;
            get_json_files(p, std::back_inserter(files));

            for (auto& c : files) {
                try {
                    candidates.insert(runtime::from_file(c));
                } catch (...) { /* Candidate invalid. */ }
            }

            if (candidates.size() > 1) {
                // If there is more than one candidate for an installation, we
                // assume that one of them is the standard runtime and the other
                // the WOW64 variant.
                files.clear();
                files.reserve(candidates.size());
                std::transform(candidates.begin(),
                    candidates.end(),
                    std::back_inserter(files),
                    [](const runtime& r) { return r.path(); });


                throw "TODO";

            } else {
                std::copy(candidates.begin(), candidates.end(), oit);
            }
        }
    }

    // Finally, transfer the runtimes to our internal vector.
    this->_runtimes.reserve(runtimes.size());
    std::copy(runtimes.begin(),
        runtimes.end(),
        std::back_inserter(this->_runtimes));
}
