// <copyright file="switcher.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "switcher.h"

#include "util.h"


/*
 * switcher::switcher
 */
switcher::switcher(void) : _handle(NULL) {
    ::ZeroMemory(&this->_status, sizeof(this->_status));
    this->_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
}


/*
 * switcher::initialise
 */
void switcher::initialise(void) {
    SID_IDENTIFIER_AUTHORITY nt_auth = SECURITY_NT_AUTHORITY;

    wil::unique_sid auth_users;
    THROW_LAST_ERROR_IF(!::AllocateAndInitializeSid(&nt_auth, 1,
        SECURITY_AUTHENTICATED_USER_RID,
        0, 0, 0, 0, 0, 0, 0,
        auth_users.put()));

    wil::unique_sid admins;
    THROW_LAST_ERROR_IF(!::AllocateAndInitializeSid(&nt_auth, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        admins.put()));

    EXPLICIT_ACCESS ea[2];
    ::ZeroMemory(&ea, sizeof(ea));

    ea[0].grfAccessPermissions = FILE_ALL_ACCESS;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea[0].Trustee.ptstrName = reinterpret_cast<LPWSTR>(admins.get());

    ea[1].grfAccessPermissions = FILE_READ_ACCESS | FILE_WRITE_ACCESS;
    ea[1].grfAccessMode = SET_ACCESS;
    ea[1].grfInheritance = NO_INHERITANCE;
    ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea[1].Trustee.ptstrName = reinterpret_cast<LPWSTR>(auth_users.get());

    wil::unique_hlocal acl;
    THROW_IF_WIN32_ERROR(::SetEntriesInAclW(static_cast<ULONG>(std::size(ea)),
        ea, nullptr, reinterpret_cast<PACL *>(acl.put())));

    SECURITY_DESCRIPTOR sd;
    THROW_LAST_ERROR_IF(!::InitializeSecurityDescriptor(&sd,
        SECURITY_DESCRIPTOR_REVISION));
    THROW_LAST_ERROR_IF(!::SetSecurityDescriptorDacl(&sd, TRUE,
        reinterpret_cast<PACL>(acl.get()), FALSE));

    SECURITY_ATTRIBUTES sa;
    ::ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = &sd;
    sa.nLength = 1;

    this->_pipe.reset(::CreateNamedPipeW(pipe_name,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
        2,
        sizeof(HRESULT),
        MAX_PATH * sizeof(wchar_t),
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL));
    THROW_LAST_ERROR_IF(!this->_pipe);

    this->_key = get_openxr_key(openxr_key, false);
    this->_wow_key = get_openxr_key(wow_key, true);

    this->_running = true;
}


/*
 * switcher::status
 */
void switcher::status(_In_ const DWORD state,
        _In_ const DWORD code,
        _In_ const DWORD controls,
        _In_ const DWORD checkpoint) {
    this->_status.dwCurrentState = state;
    this->_status.dwControlsAccepted = controls;
    this->_status.dwCheckPoint = checkpoint;

    if (code == NO_ERROR) {
        this->_status.dwWin32ExitCode = NO_ERROR;
        this->_status.dwServiceSpecificExitCode = NO_ERROR;

    } else {
        this->_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
        this->_status.dwServiceSpecificExitCode = code;
    }

    this->status();
}


/*
 * switcher::status
 */
void switcher::status(_In_ const wil::ResultException& exception,
        _In_ const DWORD checkpoint) {
    this->status(SERVICE_STOPPED, exception.GetErrorCode(), 0, checkpoint);
}


/*
 * switcher::status
 */
void switcher::status(void) {
    assert(this->_handle != NULL);
    THROW_LAST_ERROR_IF(!::SetServiceStatus(this->_handle, &this->_status));
}


/*
 * switcher::stop
 */
void switcher::stop(void) noexcept {
    this->_running.store(false, std::memory_order_release);
    // "Unblock" the connect as described on
    // https://stackoverflow.com/questions/1353263/how-to-unblock-connectnamedpipe-and-readfile-c
    wil::unique_hfile pipe(::CreateFileW(pipe_name,
        FILE_GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        NULL));
    pipe.reset();
}


/*
 * switcher::operator ()
 */
void switcher::operator ()(void) {
    std::vector<std::uint8_t> req(2 * MAX_PATH);

    while (this->_running.load(std::memory_order_acquire)) {
        ::OutputDebugString(_T("Waiting for client to connect.\r\n"));
        THROW_LAST_ERROR_IF(!::ConnectNamedPipe(this->_pipe.get(), nullptr));

        try {
            std::size_t cnt_req = 0;

            while (this->_running.load(std::memory_order_acquire)) {
                ::OutputDebugString(_T("Reading from named pipe.\r\n"));
                cnt_req += read(this->_pipe, req, cnt_req);

                // Search for consecutive binary zeros which mark the end of the
                // request.
                std::size_t end;
                if (scan(req.begin(), req.begin() + cnt_req, end)) {
                    // We have a full request, so process it.
                    auto hr = S_OK;
                    auto rt = reinterpret_cast<const wchar_t *>(req.data());
                    auto wow = rt + ::wcslen(rt) + 1;

                    try {
                        THROW_WIN32_IF(ERROR_NOT_FOUND, !::file_exists(rt));
                        wil::reg::set_value(this->_key.get(),
                            active_runtime_value,
                            rt);

                        if (this->_wow_key) {
                            if (::file_exists(wow)) {
                                wil::reg::set_value(this->_wow_key.get(),
                                    active_runtime_value,
                                    wow);

                            } else {
                                ::RegDeleteValueW(this->_wow_key.get(),
                                    active_runtime_value);
                                // This may fail if no 32-bit runtime was installed
                                // in the first place, which is fine. We therefore
                                // do not check this error.
                            }
                        }
                    } catch (wil::ResultException ex) {
                        hr = ex.GetErrorCode();
                    }

                    ::OutputDebugString(_T("Writing response.\r\n"));
                    write(this->_pipe, &hr, sizeof(hr));

                    // Erase the request we have processed, but keep whatever there
                    // might already be in the buffer from the next request.
                    const auto overflow = cnt_req - end;
                    req.erase(req.begin(), req.begin() + cnt_req);
                    req.resize(cnt_req);
                    cnt_req = overflow;

                } else {
                    // We do not have enough data to process the request.
                    req.resize(req.size() + cnt_req / 2);
                }
            }
        } catch (...) {
            ::DisconnectNamedPipe(this->_pipe.get());
        }
    }
}


/*
 * switcher::get_openxr_key
 */
wil::unique_hkey switcher::get_openxr_key(
        _In_z_ const wchar_t *path,
        _In_ const bool lenient) {
    assert(path != nullptr);
    try {
        auto retval = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, path);

        // If we the base key, get the subkey for the latest version.
        std::vector<std::wstring> versions;
        std::transform(wil::reg::key_iterator(retval.get()),
            wil::reg::key_iterator(),
            std::back_inserter(versions),
            [](const wil::reg::key_iterator::value_type& v) { return v.name; });

        // Sort such that the latest version is at the end. TODO: will break
        // at 10, I guess ...
        std::sort(versions.begin(), versions.end());
        retval = wil::reg::open_unique_key(retval.get(),
            versions.back().c_str(),
            wil::reg::key_access::readwrite);
        return retval;
    } catch (...) {
        if (lenient) {
            return wil::unique_hkey();
        } else {
            throw;
        }
    }
}


/*
 * switcher::read
 */
std::size_t switcher::read(_In_ wil::unique_hfile& handle,
        _Out_writes_bytes_(cnt) void *data,
        _In_ const std::size_t cnt) {
    auto dst = static_cast<std::uint8_t *>(data);
    auto rem = static_cast<DWORD>(cnt);
    DWORD retval;
    THROW_LAST_ERROR_IF(!::ReadFile(handle.get(), dst, rem, &retval, nullptr));
    return retval;
}


/*
 * switcher::read
 */
std::size_t switcher::read(_In_ wil::unique_hfile& handle,
        _Inout_ std::vector<std::uint8_t>& data,
        _In_ const std::size_t offset) {
    auto dst = data.data() + offset;
    auto cnt = data.size();
    if (cnt < offset) {
        return 0;
    }

    cnt -= offset;
    return read(handle, dst, cnt);
}


/*
 * switcher::write
 */
void switcher::write(_In_ wil::unique_hfile& handle,
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    auto rem = static_cast<DWORD>(cnt);
    auto src = static_cast<const std::uint8_t *>(data);

    while (rem > 0) {
        DWORD w;
        THROW_LAST_ERROR_IF(!::WriteFile(handle.get(), src, rem, &w, nullptr));
        assert(rem >= w);
        src += w;
        rem -= w;
    }
}
