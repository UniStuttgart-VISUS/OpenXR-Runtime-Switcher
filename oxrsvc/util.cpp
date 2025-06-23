// <copyright file="util.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "util.h"


/*
 * ::file_exists
 */
bool file_exists(_In_opt_z_ const wchar_t *path) noexcept {
    if (path == nullptr) {
        return false;
    } else {
        const auto attr = ::GetFileAttributesW(path);
        return ((attr != INVALID_FILE_ATTRIBUTES)
            && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0));
    }
}


/*
 * ::get_module_path
 */
std::wstring get_module_path(_In_opt_ HMODULE handle) {
    std::vector<wchar_t> buffer;
    DWORD buffer_size = MAX_PATH + 1;
    DWORD error = ERROR_SUCCESS;
    DWORD string_size = 0;

    do {
        buffer.resize(buffer_size);
        string_size = ::GetModuleFileNameW(handle,
            buffer.data(),
            buffer_size);
        THROW_LAST_ERROR_IF(string_size == 0);
        buffer_size *= 2;
    } while (error == ERROR_INSUFFICIENT_BUFFER);

    return std::wstring(buffer.data(), buffer.data() + string_size);
}
