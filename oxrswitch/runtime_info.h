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
    runtime_info(_In_z_ const wchar_t *vendor, _In_z_ const wchar_t *software);

    /// <summary>
    /// Gets a regular expression that matches the vendor name of the runtime.
    /// </summary>
    /// <returns></returns>
    inline const std::wregex& vendor(void) const noexcept {
        return this->_vendor;
    }

    /// <summary>
    /// Gets a regular expression that matches the display name of the software
    /// in the uninstall database.
    /// </summary>
    /// <returns></returns>
    inline const std::wregex& software(void) const noexcept {
        return this->_software;
    }

private:

    std::wregex _vendor;
    std::wregex _software;
};

#endif /* defined(_OXRSWITCH_RUNTIME_INFO_H) */
