// <copyright file="runtime.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_RUNTIME_H)
#define _OXRSWITCH_RUNTIME_H
#pragma once


/// <summary>
/// Represents an OpenXR runtime.
/// </summary>
class runtime final {

public:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    runtime(void) = default;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="path">The path to the JSON file holding the meta data of
    /// the runtime. If this is invalid, the resulting runtime will be
    /// invalid, too.</param>
    /// <param name="name">If not <see langword="nullptr" />, overrides the name
    /// of the runtime.</param>
    runtime(_In_ const std::wstring& path,
        _In_opt_z_ const wchar_t *name = nullptr);

    /// <summary>
    /// Answer whether the runtime is valid.
    /// </summary>
    /// <returns><see langword="true" /> if the runtime is valid,
    /// <see langword="false" /> otherwise.</returns>
    inline operator bool(void) const noexcept {
        auto f = wil::try_open_file(this->_path.c_str());
        return (!this->_name.empty() && f.file);
    }

private:

    /// <summary>
    /// Resolves the full path of <paramref name="path" />.
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static std::wstring full_path(_In_ const std::wstring& path);

    std::wstring _name;
    std::wstring _path;
};

#endif /* !defined(_OXRSWITCH_RUNTIME_H) */
