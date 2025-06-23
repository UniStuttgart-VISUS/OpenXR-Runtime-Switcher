// <copyright file="switcher.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSVC_SWITCHER_H)
#define _OXRSVC_SWITCHER_H
#pragma once


/// <summary>
/// Implements the switcher that listens on a named pipe for change requests and
/// changing the registry accordingly.
/// </summary>
class switcher final {

public:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    switcher(void);

    switcher(const switcher&) = delete;

    /// <summary>
    /// Allocates all the necessary resources.
    /// </summary>
    void initialise(void);

    /// <summary>
    /// Update the service status.
    /// </summary>
    /// <remarks>
    /// This method must not be called if the server is running in a console
    /// application.
    /// </remarks>
    /// <param name="state"></param>
    /// <param name="code"></param>
    /// <param name="controls"></param>
    /// <param name="checkpoint"></param>
    void status(_In_ const DWORD state,
        _In_ const DWORD code,
        _In_ const DWORD controls,
        _In_ const DWORD checkpoint);

    /// <summary>
    /// Update the service status.
    /// </summary>
    /// <remarks>
    /// This method must not be called if the server is running in a console
    /// application.
    /// </remarks>
    /// <param name="exception"></param>
    /// <param name="checkpoint"></param>
    void status(_In_ const wil::ResultException& exception,
        _In_ const DWORD checkpoint);

    /// <summary>
    /// Reports the service status.
    /// </summary>
    /// <remarks>
    /// This method must not be called if the server is running in a console
    /// application.
    /// </remarks>
    void status(void);

    /// <summary>
    /// Asks the service to exit by closing the named pipe.
    /// </summary>
    void stop(void) noexcept;

    /// <summary>
    /// Runs the server loop that is waiting for requests on the named pipe and
    /// changes the registry accordingly.
    /// </summary>
    void operator ()(void);

    /// <summary>
    /// Update the status handle such that <see cref="status" /> can be called.
    /// </summary>
    /// <param name="rhs"></param>
    /// <returns></returns>
    inline switcher& operator <<(const SERVICE_STATUS_HANDLE rhs) noexcept {
        this->_handle = rhs;
        return *this;
    }

    switcher& operator =(const switcher&) = delete;

private:

    /// <summary>
    /// Adjust the DACL of the given named pipe such that normal users can write
    /// to it.
    /// </summary>
    /// <param name="pipe"></param>
    static void adjust_dacl(_In_ wil::unique_hfile& pipe);

    /// <summary>
    /// Gets the registry key of the latest OpenXR installation.
    /// </summary>
    /// <param name="path">The path to the OpenXR key, which must be one of
    /// <see cref="openxr_key" /> or <paramref name="wow_key" /></param>
    /// <param name="lenient">Controls whether the method throws on error
    /// (<see langword="false" />) or returns an invalid key
    /// (<see langword="true" />).</param>
    /// <returns></returns>
    static wil::unique_hkey get_openxr_key(_In_z_ const wchar_t *path,
        _In_ const bool lenient);

    /// <summary>
    /// Read at most <paramref name="cnt" /> bytes from
    /// <paramref name="handle" />.
    /// </summary>
    /// <param name="handle"></param>
    /// <param name="data"></param>
    /// <param name="cnt"></param>
    /// <returns>The number of bytes actually read.</returns>
    static std::size_t read(_In_ wil::unique_hfile& handle,
        _Out_writes_bytes_(cnt) void *data,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Read into <paramref name="data" /> at <paramref name="offset" />.
    /// </summary>
    /// <param name="handle"></param>
    /// <param name="data"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    static std::size_t read(_In_ wil::unique_hfile& handle,
        _Inout_ std::vector<std::uint8_t>& data,
        _In_ const std::size_t offset = 0);

    /// <summary>
    /// Scans the given input range for the end of a request.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="begin"></param>
    /// <param name="end"></param>
    /// <param name="length"></param>
    /// <returns></returns>
    template<class TIterator> static bool scan(_In_ const TIterator begin,
        _In_ const TIterator end, _Out_ std::size_t& length);

    /// <summary>
    /// Write all <paramref name="cnt" /> bytes to <paramref name="handle" />.
    /// </summary>
    /// <param name="handle"></param>
    /// <param name="data"></param>
    /// <param name="cnt"></param>
    static void write(_In_ wil::unique_hfile& handle,
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt);

    /// <summary>
    /// The name of the registry value that stores the active runtime.
    /// </summary>
    static constexpr const wchar_t *const active_runtime_value
        = L"ActiveRuntime";

    /// <summary>
    /// The path in the registry where the OpenXR stuff is stored.
    /// </summary>
    static constexpr const wchar_t *const openxr_key = L"SOFTWARE\\Khronos\\"
        L"OpenXR";

    /// <summary>
    /// The name of the named pipe we use to communicate with the tool.
    /// </summary>
    static constexpr const wchar_t *const pipe_name = L"\\\\.\\pipe\\oxrswitch";

    /// <summary>
    /// The path in the registry to the 32-bit stuff of OpenXR.
    /// </summary>
    static constexpr const wchar_t *const wow_key = L"SOFTWARE\\WOW6432Node\\"
        "Khronos\\OpenXR";

    SERVICE_STATUS_HANDLE _handle;
    wil::unique_hkey _key;
    wil::unique_hfile _pipe;
    std::atomic<bool> _running;
    SERVICE_STATUS _status;
    wil::unique_hkey _wow_key;
};

#include "switcher.inl"

#endif /* !defined(_OXRSVC_SWITCHER_H) */
