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
    /// Creates a new instance from a JSON file.
    /// </summary>
    /// <param name="path">The path to the JSON file holding the meta data of
    /// the runtime.</param>
    /// <param name="wow_path">An optional path to the WOW64 version of the JSON
    /// file.</param>
    /// <param name="name">If not <see langword="nullptr" />, overrides the name
    /// of the runtime.</param>
    static runtime from_file(_In_ const std::wstring& path,
        _In_opt_z_ const wchar_t *wow_path = nullptr,
        _In_opt_z_ const wchar_t *name = nullptr);

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    runtime(void) = default;

    /// <summary>
    /// Answer the display name of the runtime.
    /// </summary>
    /// <returns>The display name of the runtime.</returns>
    inline const std::wstring& name(void) const noexcept {
        return this->_name;
    }

    /// <summary>
    /// Answer the path to the JSON file that describes the runtime.
    /// </summary>
    /// <returns>The JSON file of the runtime.</returns>
    inline const std::wstring& path(void) const noexcept {
        return this->_path;
    }

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
    /// Checks whether <paramref name="json" /> contains the required data and
    /// returns the name of the runtime according to the given file content.
    /// </summary>
    /// <param name="json"></param>
    /// <returns></returns>
    static std::wstring check_runtime(_In_ const nlohmann::json& json);

    /// <summary>
    /// Resolves the full path of <paramref name="path" />.
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static std::wstring full_path(_In_ const std::wstring& path);

    std::wstring _name;
    std::wstring _path;
    std::wstring _wow_path;
};

#endif /* !defined(_OXRSWITCH_RUNTIME_H) */
