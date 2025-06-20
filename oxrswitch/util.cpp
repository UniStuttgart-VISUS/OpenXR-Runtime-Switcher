// <copyright file="util.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "util.h"


/*
 * ::equals
 */
bool equals(_In_opt_z_ const wchar_t *lhs,
        _In_opt_z_ const wchar_t *rhs,
        _In_ const bool case_sensitive) noexcept {
    if (lhs == rhs) {
        return true;
    }

    if ((lhs == nullptr) || (rhs == nullptr)) {
        assert(lhs != rhs);
        return false;
    }

    return case_sensitive
        ? (::wcscmp(lhs, rhs) == 0)
        : (::_wcsicmp(lhs, rhs) == 0);
}


/*
 * ::equals
 */
bool equals(_In_opt_z_ const char *lhs,
        _In_opt_z_ const char *rhs,
        _In_ const bool case_sensitive) noexcept {
    if (lhs == rhs) {
        return true;
    }

    if ((lhs == nullptr) || (rhs == nullptr)) {
        assert(lhs != rhs);
        return false;
    }

    return case_sensitive
        ? (::strcmp(lhs, rhs) == 0)
        : (::_stricmp(lhs, rhs) == 0);
}


/*
 * ::load_string
 */
std::string load_string(_In_opt_ const HINSTANCE instance, _In_ const UINT id) {
    const char *str = nullptr;

    if (::LoadStringA(instance, id, reinterpret_cast<LPSTR>(&str), 0) == 0) {
        THROW_LAST_ERROR();
    }

    return str;
}


/*
 * ::load_wstring
 */
std::wstring load_wstring(_In_opt_ const HINSTANCE instance,
        _In_ const UINT id) {
    const wchar_t *str = nullptr;

    if (::LoadStringW(instance, id, reinterpret_cast<LPWSTR>(&str), 0) == 0) {
        THROW_LAST_ERROR();
    }

    return str;
}

