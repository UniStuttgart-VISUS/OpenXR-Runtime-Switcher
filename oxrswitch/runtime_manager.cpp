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
    if (::is_elevated()) {
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

    } else {
        // We are not running as administrator, so we try to use the oxrsrv to
        // perform the change.
        std::vector<wchar_t> req;
        req.reserve(runtime.path().size() + 1 + runtime.wow_path().size() + 2);

        std::copy(runtime.path().begin(),
            runtime.path().end(),
            std::back_inserter(req));
        req.push_back(L'\0');

        if (!runtime.wow_path().empty()) {
            std::copy(runtime.wow_path().begin(),
                runtime.wow_path().end(),
                std::back_inserter(req));
            req.push_back(L'\0');
        }

        req.push_back(L'\0');

        wil::unique_hfile pipe(::CreateFileW(pipe_name,
            FILE_GENERIC_READ | FILE_GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            NULL));
        THROW_LAST_ERROR_IF(!pipe);
        write(pipe, req.data(), req.size() * sizeof(wchar_t));

        HRESULT hr = S_OK;
        read(pipe, &hr, sizeof(hr));
        THROW_IF_FAILED(hr);
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
        const auto permissions = ::is_elevated()
            ? wil::reg::key_access::readwrite
            : wil::reg::key_access::read;
        retval = wil::reg::open_unique_key(retval.get(),
            versions.back().c_str(),
            permissions);
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
 * runtime_manager::read
 */
void runtime_manager::read(_In_ wil::unique_hfile& handle,
        _Out_writes_bytes_(cnt) void *data,
        _In_ const std::size_t cnt) {
    auto dst = static_cast<std::uint8_t *>(data);
    auto rem = static_cast<DWORD>(cnt);

    while (rem > 0) {
        DWORD r;
        THROW_LAST_ERROR_IF(!::ReadFile(handle.get(), dst, rem, &r, nullptr));
        assert(rem >= r);
        dst += r;
        rem -= r;
    }
}


/*
 * runtime_manager::write
 */
void runtime_manager::write(_In_ wil::unique_hfile &handle,
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    auto rem = static_cast<DWORD>(cnt);
    auto src = static_cast<const std::uint8_t *>(data);

    while (rem > 0) {
        DWORD w;
        THROW_LAST_ERROR_IF(!::WriteFile(handle.get(), src, rem, &w, nullptr));
        assert(rem >= w);
        src += w;
        rem -= w;
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
            auto n = ::load_wstring(NULL, IDS_WMR);

            if (::file_exists(w)) {
                *oit++ = runtime::from_file(p, w, n);
            } else {
                *oit++ = runtime::from_file(p, nullptr, n.c_str());
            }
        }
    }

    // Third, look for runtimes in known installation paths.
    {
        const auto is_32bit = [](const runtime& r) {
            return ::contains(r.path(), L"32", false)
                || ::contains(r.path(), L"x86", false)
                || ::contains(r.path(), L"i386", false);
        };

        const auto is_64bit = [](const runtime& r) {
            return ::contains(r.path(), L"64", false)
                || ::contains(r.path(), L"x64", false)
                || ::contains(r.path(), L"amd64", false);
        };

        std::set<std::wstring, path_compare> installs;
        get_uninstall_paths(std::inserter(installs, installs.begin()));
        get_software_paths(std::inserter(installs, installs.begin()));

        //installs.insert(std::wstring(L"\\\\villanella\\c$\\Program Files\\Oculus"));

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
                // the WOW64 variant. In a first step, we copy the candidates to
                // a vector such that we can partition them according to their
                // runtime names. This way, we can find the files that belong
                // together as a native 64 bit and WOW64 pair.
                std::vector<runtime> rem_candidates;
                rem_candidates.reserve(candidates.size());
                std::copy(candidates.begin(),
                    candidates.end(),
                    std::back_inserter(rem_candidates));

                // Check the candidates for 32/64 pairs.
                for (auto it = rem_candidates.begin();
                        it != rem_candidates.end();) {
                    // 'split' is the first runtime in 'rem_candidates' that has
                    // a different name than 'it'.
                    const auto split = std::partition(it, rem_candidates.end(),
                        [&it](const runtime& r) {
                            return (it->name() == r.name());
                        });

                    if (it != split) {
                        // Found a pair with matching names.
                        const auto jt = it + 1;
                        const auto it32 = is_32bit(*it);
                        const auto it64 = is_64bit(*it);
                        const auto jt32 = is_32bit(*jt);
                        const auto jt64 = is_64bit(*jt);

                        if (it64 && jt32) {
                            // 'it' is native 64 bit, 'jt' is 32 bit.
                            *oit++ = runtime::from_file(it->path(), jt->path(),
                                it->name());

                        } else if (jt64 && it32) {
                            // 'it' is native 64 bit, 'jt' is 32 bit.
                            *oit++ = runtime::from_file(jt->path(), it->path(),
                                jt->name());

                        } else if (it64) {
                            // Have only 64 bit and no matching 32 bit.
                            *oit++ = *it;

                        } else if (jt64) {
                            // Have only 64 bit and no matching 32 bit.
                            *oit++ = *jt;

                        } else {
                            // Just copy everyhing.
                            oit = std::copy(it, split, *oit);
                        }

                        it = rem_candidates.erase(it, split);

                    } else {
                        // No match found, just copy everything.
                        oit = std::copy(it, split, oit);
                    }
                }
            } else {
                // There was only one candidate in the folder, which we add.
                oit = std::copy(candidates.begin(), candidates.end(), oit);
            }
        }
    }

    // Finally, transfer the runtimes to our internal vector.
    this->_runtimes.reserve(runtimes.size());
    std::copy(runtimes.begin(),
        runtimes.end(),
        std::back_inserter(this->_runtimes));
}
