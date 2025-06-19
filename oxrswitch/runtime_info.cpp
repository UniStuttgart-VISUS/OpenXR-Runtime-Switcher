// <copyright file="runtime_info.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime_info.h"


/// <summary>
/// The flags we use for all regular expressions.
/// </summary>
static constexpr auto regex_flags = std::wregex::icase
    | std::wregex::optimize
    | std::wregex::ECMAScript;


/*
 * runtime_info::oculus
 */
const std::vector<runtime_info> runtime_info::runtimes = {
    runtime_info(L"^oculus", L"oculus"),
    runtime_info(L"^valve", L"steamvr"),
    runtime_info(L"^varjo", L"runtime", nullptr, L"InstallDir"),
    runtime_info(L"^htc", L"updater", nullptr, L"AppPath"),
};


/*
 * runtime_info::runtime_info
 */
runtime_info::runtime_info(_In_z_ const wchar_t *vendor,
        _In_z_ const wchar_t *software,
        _In_opt_z_ const wchar_t *subkey,
        _In_opt_z_ const wchar_t *value)
    : _software(software, regex_flags),
        _subkey((subkey != nullptr) ? subkey : L""),
        _value((value != nullptr) ? value : L""),
        _vendor(vendor, regex_flags) { }


/*
 * runtime_info::is_match
 */
bool runtime_info::is_match(_In_ const std::wstring& vendor,
        _In_ const std::wstring& software) const noexcept {
    return (std::regex_match(vendor, this->_vendor)
        && std::regex_match(software, this->_software));
}


/*
 * runtime_info::try_get_installation_path
 */
_Success_(return) bool runtime_info::try_get_installation_path(
        _In_ const wil::unique_hkey& key,
        _Out_ std::wstring& path) const {
    auto value = this->_value.empty() ? nullptr : this->_value.c_str();

    if (!this->_subkey.empty()) {
        try {
            auto k = wil::reg::open_unique_key(key.get(),
                this->_subkey.c_str());
            path = wil::reg::get_value_expanded_string(k.get(), value);
            return true;
        } catch (...) {
            return false;
        }

    } else {
        try {
            path = wil::reg::get_value_expanded_string(key.get(), value);
            return true;
        } catch (...) {
            return false;
        }
    }
}
