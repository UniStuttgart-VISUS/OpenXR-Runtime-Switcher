// <copyright file="runtime_info.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_RUNTIME_INFO_H)
#define _OXRSWITCH_RUNTIME_INFO_H
#pragma once


/// <summary>
/// The runtime information that describes the properties of a known OpenXR
/// runtime and how to identify it.
/// </summary>
class runtime_info final {

public:

    /// <summary>
    /// Enumerates all known runtimes.
    /// </summary>
    static const std::vector<runtime_info> runtimes;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="vendor">A regular expression the software vendor in the
    /// registry must match.</param>
    /// <param name="software">A regular expression the display name of the
    /// software in the registry must match.</param>
    runtime_info(_In_z_ const wchar_t *vendor,
        _In_z_ const wchar_t *software,
        _In_opt_z_ const wchar_t *subkey = nullptr,
        _In_opt_z_ const wchar_t *value = nullptr);

    /// <summary>
    /// Answer whether the given <paramref name="vendor" /> and
    /// <paramref name="software" /> match the description of the runtime.
    /// </summary>
    /// <param name="vendor"></param>
    /// <param name="software"></param>
    /// <returns></returns>
    bool is_match(_In_ const std::wstring& vendor,
        _In_ const std::wstring& software) const noexcept;

    /// <summary>
    /// Gets a regular expression that matches the display name of the software
    /// in the uninstall database.
    /// </summary>
    /// <returns></returns>
    inline const std::wregex& software(void) const noexcept {
        return this->_software;
    }

    /// <summary>
    /// Tries to derive the installation path from the custom software key of
    /// the registry.
    /// </summary>
    /// <param name="key">The vendor-specific software key in the
    /// &quot;Software&quot; or &quot;WOW6432Node&quot; location of the
    /// registry.</param>
    /// <param name="path">Receives the installation path in case of success.
    /// </param>
    /// <returns></returns>
    _Success_(return) bool try_get_installation_path(
        _In_ const wil::unique_hkey& key,
        _Out_ std::wstring& path) const;

    /// <summary>
    /// Gets a regular expression that matches the vendor name of the runtime.
    /// </summary>
    /// <returns></returns>
    inline const std::wregex& vendor(void) const noexcept {
        return this->_vendor;
    }

private:

    std::wregex _software;
    std::wstring _subkey;
    std::wstring _value;
    std::wregex _vendor;
};

#endif /* defined(_OXRSWITCH_RUNTIME_INFO_H) */
