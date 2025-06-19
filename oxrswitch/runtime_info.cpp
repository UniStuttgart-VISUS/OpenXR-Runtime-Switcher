// <copyright file="runtime_info.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime_info.h"


/// <summary>
/// The flags we use for all regular expressions.
/// </summary>
static constexpr auto regex_flags = std::wregex::icase
    | std::wregex::optimize
    | std::wregex::ECMAScript;


/*
 * runtime_info::oculus
 */
const std::vector<runtime_info> runtime_info::runtimes = {
    runtime_info(L"^oculus", L"oculus"),
    runtime_info(L"^valve", L"steamvr"),
    runtime_info(L"^varjo", L"varjo")
};


/*
 * runtime_info::runtime_info
 */
runtime_info::runtime_info(_In_z_ const wchar_t *vendor,
        _In_z_ const wchar_t *software) 
    : _software(software, regex_flags), _vendor(vendor, regex_flags) { }
