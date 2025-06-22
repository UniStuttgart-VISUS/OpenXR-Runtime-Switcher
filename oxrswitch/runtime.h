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
    /// Creates a new instance from a JSON file.
    /// </summary>
    /// <param name="path">The path to the JSON file holding the meta data of
    /// the runtime.</param>
    /// <param name="wow_path">The path to the WOW64 version of the JSON file.
    /// </param>
    /// <param name="name">If not <see langword="nullptr" />, overrides the name
    /// of the runtime.</param>
    inline static runtime from_file(_In_ const std::wstring& path,
            _In_ const std::wstring& wow_path,
            _In_opt_z_ const wchar_t *name = nullptr) {
        return runtime::from_file(path,
            wow_path.empty() ? nullptr : wow_path.c_str(),
            name);
    }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    runtime(void) = default;

    /// <summary>
    /// Initialises a clone of <paramref name="other" />.
    /// </summary>
    /// <param name="other"></param>
    runtime(_In_ const runtime& other) = default;

    /// <summary>
    /// Initialises by moving <paramref name="other" />.
    /// </summary>
    /// <param name="other"></param>
    runtime(_Inout_ runtime&& other) noexcept
        : _name(std::move(other._name)),
        _path(std::move(other._path)),
        _wow_path(std::move(other._wow_path)) { }

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
    /// If a runtime path for WOW64 is set, answer this path.
    /// </summary>
    /// <returns>The JSON file of the runtime or an empty string if no
    /// WOW64 path is available.</returns>
    inline const std::wstring& wow_path(void) const noexcept {
        return this->_wow_path;
    }

    /// <summary>
    /// Assignment.
    /// </summary>
    /// <param name="rhs"></param>
    runtime& operator =(_In_ const runtime& rhs) = default;

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs"></param>
    runtime& operator =(_Inout_ runtime&& rhs) noexcept;

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


/// <summary>
/// Provides a ordering for runtimes based on the path of the runtime file and
/// nothing else.
/// </summary>
template<> struct std::less<runtime> {

    inline bool operator()(_In_ const runtime& lhs,
            _In_ const runtime& rhs) const noexcept {
        return (lhs.path() < rhs.path());
    }
};


#endif /* !defined(_OXRSWITCH_RUNTIME_H) */
