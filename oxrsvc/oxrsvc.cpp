// <copyright file="oxrsvc.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"

#include "service.h"
#include "switcher.h"


/// <summary>
/// Handles service control requests by the service control manager.
/// </summary>
/// <param name="control"></param>
/// <param name="eventType"></param>
/// <param name="eventData"></param>
/// <param name="context"></param>
/// <returns></returns>
DWORD WINAPI service_control_handler(_In_ const DWORD control,
        _In_ const DWORD eventType,
        _In_opt_ void *eventData,
        _In_opt_ void *context) {
    auto retval = NO_ERROR;
    auto s = static_cast<switcher *>(context);

    switch (control) {
        case SERVICE_CONTROL_INTERROGATE:
            try {
                s->status();
            } catch (wil::ResultException ex) {
                retval = ex.GetErrorCode();
            }
            break;

        case SERVICE_CONTROL_STOP:
            s->stop();
            break;

        default:
            retval = ERROR_CALL_NOT_IMPLEMENTED;
            break;
    }

    return retval;
}


/// <summary>
/// Entry point of the service.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
void WINAPI service_main(const DWORD argc, wchar_t **argv) {
    switcher switcher;

    // Register the service control handler.
    auto handle = ::RegisterServiceCtrlHandlerExW(::service_name,
        ::service_control_handler, &switcher);
    THROW_LAST_ERROR_IF_NULL(handle);
    switcher << handle;

    try {
        // Notify the service manager we are starting in our own process.
        switcher.status(SERVICE_START_PENDING, 0, 0, 1);
        switcher.initialise();

        // Run the server process.
        switcher.status(SERVICE_RUNNING, NO_ERROR, SERVICE_ACCEPT_STOP, 2);
        switcher();
        switcher.status(SERVICE_STOPPED, NO_ERROR, 0, 4);
    } catch (wil::ResultException ex) {
        switcher.status(ex, 5);
    }
}

/// <summary>
/// Entry point of the application.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int wmain(_In_ const int argc, _In_reads_(argc) const wchar_t **argv) {
    SERVICE_TABLE_ENTRYW service_table[] = {
        { const_cast<wchar_t *>(::service_name), service_main },
        { nullptr, nullptr}
    };

    if (!::StartServiceCtrlDispatcherW(service_table)) {
        auto error = ::GetLastError();
        if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            // If we are here, we are not running as a service, so we either
            // install the service for testing, uninstall it or run it in
            // foreground for debugging.

            try {
                if ((argc > 1) && (::_wcsicmp(argv[1], L"/install") == 0)) {
                    // Install the service for testing purposes.
                    //::install_event_source();
                    ::install_service(::service_name, L"OpenXR Runtime "
                        L"Switcher (self-registered)");

                } else if ((argc > 1) && (::_wcsicmp(argv[1], L"/uninstall")
                    == 0)) {
                    // Uninstall the service.
                    //::uninstall_event_source();
                    ::uninstall_service(::service_name);

                } else {
                    // Initialise the server from the registry and run it as
                    // foreground application.
                    switcher switcher;
                    ::OutputDebugString(_T("Initialise switcher.\r\n"));
                    switcher.initialise();
                    ::OutputDebugString(_T("Run switcher.\r\n"));
                    switcher();
                }
            } catch (wil::ResultException ex) {
                std::cerr << ex.what() << std::endl;
            }
        }
    }
}
