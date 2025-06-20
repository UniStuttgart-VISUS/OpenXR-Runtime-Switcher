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

