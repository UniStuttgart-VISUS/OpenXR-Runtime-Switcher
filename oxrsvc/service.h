// <copyright file="service.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSVC_SERVICE_H)
#define _OXRSVC_SERVICE_H
#pragma once


/// <summary>
/// The name we use to register our service.
/// </summary>
constexpr const wchar_t *const service_name = L"oxrsvc";

/// <summary>
/// Installs an executable (or the executing module if
/// <paramref name="binary_path" /> is <c>null</c>) as a Windows service.
/// </summary>
/// <param name="service_name"></param>
/// <param name="display_name"></param>
/// <param name="binary_path"></param>
/// <param name="start_type"></param>
/// <param name="service_type"></param>
/// <param name="error_control"></param>
void install_service(_In_ const std::wstring& service_name,
    _In_ const std::wstring& display_name,
    _In_opt_z_ const wchar_t *binary_path = nullptr,
    _In_ const DWORD start_type = SERVICE_AUTO_START,
    _In_ const DWORD service_type = SERVICE_WIN32_OWN_PROCESS,
    _In_ const DWORD error_control = SERVICE_ERROR_NORMAL);

/// <summary>
/// Uninstalls the given Windows service.
/// </summary>
/// <param name="service_name"></param>
void uninstall_service(_In_ const std::wstring &service_name);

#endif /* !defined(_OXRSVC_SERVICE_H) */
