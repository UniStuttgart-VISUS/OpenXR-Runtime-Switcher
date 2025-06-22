// <copyright file="path_compare.cpp" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2025 Visualisierungsinstitut der Universit�t Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph M�ller</author>

#include "pch.h"
#include "path_compare.h"


/*
 * path_compare::operator ()
 */
bool path_compare::operator ()(_In_ const std::wstring& lhs,
        _In_ const std::wstring& rhs) const noexcept {
#if defined(_WIN32)
    return (::_wcsicmp(lhs.c_str(), rhs.c_str()) < 0);
#else /* defined(_WIN32) */
    return (::wcscmp(lhs.c_str(), rhs.c_str()) < 0);
#endif /* defined(_WIN32) */
}


/*
 * path_compare::operator ()
 */
bool path_compare::operator ()(_In_ const std::string& lhs,
        _In_ const std::string& rhs) const noexcept {
#if defined(_WIN32)
    return (::_stricmp(lhs.c_str(), rhs.c_str()) < 0);
#else /* defined(_WIN32) */
    return (::strcmp(lhs.c_str(), rhs.c_str()) < 0);
#endif /* defined(_WIN32) */
}
