// <copyright file="util.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_UTIL_H)
#define _OXRSWITCH_UTIL_H
#pragma once


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
/// Running whether the calling process is running elevated (as administrator).
/// </summary>
/// <returns></returns>
bool is_elevated(void);

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
