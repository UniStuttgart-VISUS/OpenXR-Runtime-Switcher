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
 * ::is_elevated
 */
bool is_elevated(void) {
    wil::unique_handle token;
    THROW_LAST_ERROR_IF(!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY,
        token.put()));

    TOKEN_ELEVATION info;
    DWORD size;
    THROW_LAST_ERROR_IF(!::GetTokenInformation(token.get(), TokenElevation,
        &info, sizeof(info), &size));

    return (info.TokenIsElevated != FALSE);
}


/*
 * ::load_string
 */
std::string load_string(_In_opt_ const HINSTANCE instance, _In_ const UINT id) {
    // Note: As we are building with UNICODE, we cannot obtain the read-only
    // pointer to the actual resource here, but we must retrieve it and convert
    // the result.
    auto resource = ::load_wstring(instance, id);

    std::size_t cnt = 0;
    ::wcstombs_s(&cnt,
        nullptr, 0,
        resource.c_str(), resource.size() * sizeof(wchar_t));
    std::string retval(cnt, L'\0');

    ::wcstombs_s(&cnt,
        retval.data(), retval.size(),
        resource.c_str(), resource.size() * sizeof(wchar_t));
    retval.resize(cnt);

    return retval;
}


/*
 * ::load_wstring
 */
std::wstring load_wstring(_In_opt_ const HINSTANCE instance,
        _In_ const UINT id) {
    const wchar_t *str = nullptr;
    const auto len = ::LoadStringW(instance, id, reinterpret_cast<LPWSTR>(&str),
        0);
    THROW_LAST_ERROR_IF(len == 0);
    return std::wstring(str, len);
}
