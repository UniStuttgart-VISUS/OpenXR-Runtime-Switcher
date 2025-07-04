// <copyright file="util.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2025 Visualisierungsinstitut der Universit�t Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph M�ller</author>

#if !defined(_OXRSWITCH_UTIL_H)
#define _OXRSWITCH_UTIL_H
#pragma once

/// <summary>
/// Combine two paths.
/// </summary>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns></returns>
std::wstring combine_path(_In_ const std::wstring& left,
    _In_opt_z_ const wchar_t *right);

/// <summary>
/// Answer whether <paramref name="haystack" /> contains
/// <paramref name="needle" />.
/// </summary>
/// <param name="haystack"></param>
/// <param name="needle"></param>
/// <param name="case_sensitive"></param>
/// <returns></returns>
bool contains(_In_opt_z_ const wchar_t *haystack,
    _In_opt_z_ const wchar_t *needle,
    _In_ const bool case_sensitive = true) noexcept;

/// <summary>
/// Answer whether <paramref name="haystack" /> contains
/// <paramref name="needle" />.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <typeparam name="TTraits"></typeparam>
/// <typeparam name="TAlloc"></typeparam>
/// <param name="haystack"></param>
/// <param name="needle"></param>
/// <param name="case_sensitive"></param>
/// <returns></returns>
template<class TChar, class TTraits, class TAlloc>
bool contains(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& haystack,
        _In_opt_z_ const wchar_t *needle,
        _In_ const bool case_sensitive = true) noexcept {
    return ::contains(haystack.c_str(), needle, case_sensitive);
}

/// <summary>
/// Answer whether the <paramref name="path" /> designates an existing
/// directory.
/// </summary>
/// <param name="path">An optional path to a directory.</param>
/// <returns><see langword="true" /> if <paramref name="path" /> is valid and
/// designates an existing directory, <see langword="false" /> otherwise.
/// </returns>
bool directory_exists(_In_opt_z_ const wchar_t *path) noexcept;

/// <summary>
/// Answer whether the <paramref name="path" /> designates an existing
/// directory.
/// </summary>
/// <param name="path">An optional path to a directory.</param>
/// <returns><see langword="true" /> if <paramref name="path" /> is valid and
/// designates an existing directory, <see langword="false" /> otherwise.
/// </returns>
bool directory_exists(_In_opt_z_ const char *path) noexcept;

/// <summary>
/// Answer whether the <paramref name="path" /> designates an existing
/// directory.
/// </summary>
/// <param name="path">An optional path to a directory.</param>
/// <returns><see langword="true" /> if <paramref name="path" /> is valid and
/// designates an existing directory, <see langword="false" /> otherwise.
/// </returns>
template<class TChar, class TTraits, class TAlloc>
bool directory_exists(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& path) noexcept {
    return ::directory_exists(path.c_str());
}

/// <summary>
/// Answer whether <paramref name="lhs" /> ends with <paramref name="rhs" />.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <typeparam name="TTraits"></typeparam>
/// <typeparam name="TAlloc"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template<class TChar, class TTraits, class TAlloc>
inline bool ends_with(_In_ const std::basic_string<TChar, TTraits, TAlloc>& lhs,
        _In_opt_z_ const TChar *rhs,
        _In_ const bool case_sensitive = true) noexcept {
    const auto len = (rhs != nullptr) ? TTraits::length(rhs) : 0;
    return (lhs.size() >= len)
        && std::equal(lhs.begin() + lhs.size() - len, lhs.end(), rhs);
}

/// <summary>
/// Answer whether <paramref name="lhs" /> and <paramref name="rhs" /> are equal
/// strings.
/// </summary>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <param name="case_sensitive"></param>
/// <returns></returns>
bool equals(_In_opt_z_ const wchar_t *lhs,
    _In_opt_z_ const wchar_t *rhs,
    _In_ const bool case_sensitive = true) noexcept;

/// <summary>
/// Answer whether <paramref name="lhs" /> and <paramref name="rhs" /> are equal
/// strings.
/// </summary>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <param name="case_sensitive"></param>
/// <returns></returns>
bool equals(_In_opt_z_ const char *lhs,
    _In_opt_z_ const char *rhs,
    _In_ const bool case_sensitive = true) noexcept;

/// <summary>
/// Answer whether <paramref name="lhs" /> and <paramref name="rhs" /> are equal
/// strings.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <typeparam name="TLTraits"></typeparam>
/// <typeparam name="TLAlloc"></typeparam>
/// <typeparam name="TRAlloc"></typeparam>
/// <typeparam name="TRTraits"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <param name="case_sensitive"></param>
/// <returns></returns>
template<class TChar,
    class TLTraits, class TRTraits,
    class TLAlloc, class TRAlloc>
inline bool equals(_In_ const std::basic_string<TChar, TLTraits, TLAlloc>& lhs,
        _In_ const std::basic_string<TChar, TRTraits, TRAlloc>& rhs,
        _In_ const bool case_sensitive = true) noexcept {
    return ::equals(lhs.c_str(), rhs.c_str(), case_sensitive);
}

/// <summary>
/// Answer whether <paramref name="lhs" /> and <paramref name="rhs" /> are equal
/// strings.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <typeparam name="TTraits"></typeparam>
/// <typeparam name="TAlloc"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <param name="case_sensitive"></param>
/// <returns></returns>
template<class TChar, class TTraits, class TAlloc>
inline bool equals(_In_ const std::basic_string<TChar, TTraits, TAlloc>& lhs,
        _In_opt_z_ const TChar *rhs,
        _In_ const bool case_sensitive = true) noexcept {
    return ::equals(lhs.c_str(), rhs, case_sensitive);
}

/// <summary>
/// Answer whether <paramref name="lhs" /> and <paramref name="rhs" /> are equal
/// strings.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <typeparam name="TTraits"></typeparam>
/// <typeparam name="TAlloc"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template<class TChar, class TTraits, class TAlloc>
inline bool equals(_In_opt_z_ const TChar *lhs,
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& rhs,
        _In_ const bool case_sensitive = true) noexcept {
    return ::equals(lhs, rhs.c_str(), case_sensitive);
}

/// <summary>
/// Expands all environment variables in the given string.
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::wstring expand_environment_variables(_In_z_ const wchar_t *str);

/// <summary>
/// Answer whether the <paramref name="path" /> designates an existing
/// file.
/// </summary>
/// <param name="path">An optional path to a file.</param>
/// <returns><see langword="true" /> if <paramref name="path" /> is valid and
/// designates an existing file, <see langword="false" /> otherwise.
/// </returns>
bool file_exists(_In_opt_z_ const wchar_t *path) noexcept;

/// <summary>
/// Answer whether the <paramref name="path" /> designates an existing
/// file.
/// </summary>
/// <param name="path">An optional path to a file.</param>
/// <returns><see langword="true" /> if <paramref name="path" /> is valid and
/// designates an existing file, <see langword="false" /> otherwise.
/// </returns>
bool file_exists(_In_opt_z_ const char *path) noexcept;

/// <summary>
/// Answer whether the <paramref name="path" /> designates an existing
/// file.
/// </summary>
/// <param name="path">An optional path to a file.</param>
/// <returns><see langword="true" /> if <paramref name="path" /> is valid and
/// designates an existing file, <see langword="false" /> otherwise.
/// </returns>
template<class TChar, class TTraits, class TAlloc>
bool file_exists(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& path) noexcept {
    return ::file_exists(path.c_str());
}

/// <summary>
/// Gets the path to the file holding the given module.
/// </summary>
/// <param name="handle"></param>
/// <returns></returns>
std::wstring get_module_path(_In_opt_ HMODULE handle);

/// <summary>
/// Answer whether <paramref name="c" /> is a directory separator.
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
inline bool is_directory_separator(_In_ const wchar_t c) noexcept {
#if defined(_WIN32)
    return (c == L'\\') || (c == L'/');
#else /* defined(_WIN32) */
    return (c == L'/');
#endif /* defined(_WIN32) */
}

/// <summary>
/// Answer whether <paramref name="c" /> is a directory separator.
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
inline bool is_directory_separator(_In_ const char c) noexcept {
#if defined(_WIN32)
    return (c == '\\') || (c == '/');
#else /* defined(_WIN32) */
    return (c == '/');
#endif /* defined(_WIN32) */
}

/// <summary>
/// Running whether the calling process is running elevated (as administrator).
/// </summary>
/// <returns></returns>
bool is_elevated(void);

/// <summary>
/// Answer whether the given two paths are the same directory.
/// </summary>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
bool is_same_directory(_In_ const std::wstring& lhs,
    _In_ const std::wstring& rhs) noexcept;

/// <summary>
/// Loads the specified string resource.
/// </summary>
/// <param name="instance"></param>
/// <param name="id"></param>
/// <returns></returns>
std::string load_string(_In_opt_ const HINSTANCE instance, _In_ const UINT id);

/// <summary>
/// Loads the specified string resource.
/// </summary>
/// <param name="instance"></param>
/// <param name="id"></param>
/// <returns></returns>
std::wstring load_wstring(_In_opt_ const HINSTANCE instance,
    _In_ const UINT id);

#endif /* !defined(_OXRSWITCH_UTIL_H) */
