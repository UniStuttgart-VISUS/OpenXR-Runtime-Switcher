// <copyright file="util.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSVC_UTIL_H)
#define _OXRSVC_UTIL_H
#pragma once


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
/// Gets the path to the file holding the given module.
/// </summary>
/// <param name="handle"></param>
/// <returns></returns>
std::wstring get_module_path(_In_opt_ HMODULE handle);

#endif /* !defined(_OXRSVC_UTIL_H) */
