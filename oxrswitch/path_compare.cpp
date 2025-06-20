// <copyright file="path_compare.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "path_compare.h"


/*
 * path_compare::operator ()
 */
bool path_compare::operator ()(_In_ const std::wstring& lhs,
        _In_ const std::wstring& rhs) const noexcept {
    return ::_wcsicmp(lhs.c_str(), rhs.c_str());
}


/*
 * path_compare::operator ()
 */
bool path_compare::operator ()(_In_ const std::string& lhs,
        _In_ const std::string& rhs) const noexcept {
    return ::_stricmp(lhs.c_str(), rhs.c_str());
}
