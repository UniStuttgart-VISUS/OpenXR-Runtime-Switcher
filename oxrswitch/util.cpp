// <copyright file="util.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "util.h"


/*
 * ::contains
 */
bool contains(_In_opt_z_ const wchar_t *haystack,
        _In_opt_z_ const wchar_t *needle,
            _In_ const bool case_sensitive) noexcept {
    const auto hastack_empty = (haystack == nullptr) || (*haystack == 0);
    const auto needle_empty = (needle == nullptr) || (*needle == 0);

    if (hastack_empty) {
        // If the input is empty, the search string must be empty, too.
        return needle_empty;
    }

    if (needle_empty) {
        // An empty string is contained in any string.
        return true;
    }

    if (case_sensitive) {
        // Can use library function for case-sensitive search.
        return (::wcsstr(haystack, needle) != nullptr);
    }

    // Search on our own.
    const auto needle_len = ::wcslen(needle);
    for (auto h = haystack; *h; ++h) {
        std::size_t i = 0;
        for (; i < needle_len; ++i) {
            if (std::tolower(h[i]) != std::tolower(needle[i])) {
                break;
            }
            if (i + 1 == needle_len) {
                return true;
            }
        }
    }

    return false;
}


/*
 * ::directory_exists
 */
bool directory_exists(_In_opt_z_ const wchar_t *path) noexcept {
    if (path == nullptr) {
        return false;
    } else {
        const auto attr = ::GetFileAttributesW(path);
        return ((attr != INVALID_FILE_ATTRIBUTES) 
            && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0));
    }
}

/*
 * ::directory_exists
 */
bool directory_exists(_In_opt_z_ const char *path) noexcept {
    if (path == nullptr) {
        return false;
    } else {
        const auto attr = ::GetFileAttributesA(path);
        return ((attr != INVALID_FILE_ATTRIBUTES) 
            && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0));
    }
}


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
 * ::expand_environment_variables
 */
std::wstring expand_environment_variables(_In_z_ const wchar_t *str) {
    if (str == nullptr) {
        return std::wstring();
    }

    std::wstring retval(2, L'\0');

    while (true) {
        auto cnt = ::ExpandEnvironmentStringsW(str,
            retval.data(),
            static_cast<DWORD>(retval.size()));

        if (cnt <= retval.size()) {
            retval.resize(cnt - 1);
            return retval;
        }

        retval.resize(cnt);
    }
}


/*
 * ::file_exists
 */
bool file_exists(_In_opt_z_ const wchar_t *path) noexcept {
    if (path == nullptr) {
        return false;
    } else {
        const auto attr = ::GetFileAttributesW(path);
        return ((attr != INVALID_FILE_ATTRIBUTES)
            && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0));
    }
}


/*
 * ::file_exists
 */
bool file_exists(_In_opt_z_ const char *path) noexcept {
    if (path == nullptr) {
        return false;
    } else {
        const auto attr = ::GetFileAttributesA(path);
        return ((attr != INVALID_FILE_ATTRIBUTES)
            && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0));
    }
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
 * ::is_same_directory
 */
bool is_same_directory(_In_ const std::wstring& lhs,
        _In_ const std::wstring& rhs) noexcept {
    const auto l = std::find_if(lhs.rbegin(), lhs.rend(),
        [](const wchar_t c) { return ::is_directory_separator(c); });
    const auto r = std::find_if(rhs.rbegin(), rhs.rend(),
        [](const wchar_t c) { return ::is_directory_separator(c); });
    return std::equal(lhs.begin(), l.base(),
        rhs.begin(), r.base(),
        [](const wchar_t l, const wchar_t r) {
#if defined(_WIN32)
            return (std::tolower(l) == std::tolower(r));
#else /* defined(_WIN32) */
            return (l == r);
#endif /* defined(_WIN32) */
        });
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
