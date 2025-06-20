// <copyright file="path_compare.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2025 Visualisierungsinstitut der Universit�t Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph M�ller</author>

#pragma once


/// <summary>
/// Compares to paths.
/// </summary>
struct path_compare final {

    bool operator ()(_In_ const std::wstring& lhs,
        _In_ const std::wstring& rhs) const noexcept;

    bool operator ()(_In_ const std::string& lhs,
        _In_ const std::string& rhs) const noexcept;

};
