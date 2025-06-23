// <copyright file="service.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "service.h"

#include "util.h"


/*
 * ::install_service
 */
void ::install_service(_In_ const std::wstring& service_name,
        _In_ const std::wstring& display_name,
        _In_opt_z_ const wchar_t *binary_path,
        _In_ const DWORD start_type,
        _In_ const DWORD service_type,
        _In_ const DWORD error_control) {
    wil::unique_schandle svc_mgr(::OpenSCManagerW(nullptr, nullptr,
        SC_MANAGER_ALL_ACCESS));
    THROW_LAST_ERROR_IF_NULL(svc_mgr);

    std::wstring module_path;
    if (binary_path == nullptr) {
        module_path = get_module_path(NULL);
        binary_path = module_path.data();
    }

    wil::unique_schandle svc(::CreateServiceW(svc_mgr.get(),
        service_name.c_str(),
        display_name.c_str(),
        SERVICE_ALL_ACCESS,
        service_type,
        start_type,
        error_control,
        binary_path,
        NULL, NULL, NULL, NULL, NULL));
    THROW_LAST_ERROR_IF_NULL(svc);
}



/*
 * ::uninstall_service
 */
void uninstall_service(_In_ const std::wstring& service_name) {
    wil::unique_schandle svc_mgr(::OpenSCManagerW(nullptr, nullptr,
        SC_MANAGER_ALL_ACCESS));
    THROW_LAST_ERROR_IF_NULL(svc_mgr);

    wil::unique_schandle svc(::OpenServiceW(svc_mgr.get(),
        service_name.c_str(),
        SERVICE_ALL_ACCESS));
    THROW_LAST_ERROR_IF_NULL(svc);

    THROW_LAST_ERROR_IF(!::DeleteService(svc.get()));
}

