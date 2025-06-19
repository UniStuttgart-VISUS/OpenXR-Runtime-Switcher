// <copyright file="runtime.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime.h"


/*
 * runtime::runtime
 */
runtime::runtime(_In_ const std::wstring& path,
        _In_opt_z_ const wchar_t *name)
        : _path(full_path(path)) {
    if (name != nullptr) {
        this->_name = name;
    }

    if (this->_name.empty()) try {
        std::ifstream f(this->_path);
        const auto data = nlohmann::json::parse(f);

        auto it = data.find("runtime");
        auto end = data.end();
        if (it != data.end()) {
            end = it->end();
            it = it->find("name");
        }

        if (it != end) {
            auto name = it->template get<std::string>();

            std::size_t cnt = 0;
            ::mbstowcs_s(&cnt,
                nullptr, 0,
                name.c_str(), name.size());
            this->_name.resize(cnt);

            ::mbstowcs_s(&cnt,
                this->_name.data(), this->_name.size(),
                name.c_str(), _TRUNCATE);
            this->_name.resize(cnt);
        }
    } catch (...) { /* We do not fail, but yield an invalid runtime. */ }
}


/*
 * runtime::full_path
 */
std::wstring runtime::full_path(_In_ const std::wstring& path) {
    auto expected = ::GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
    THROW_LAST_ERROR_IF(expected == 0);

    std::wstring retval(expected, L'\0');
    auto actual = ::GetFullPathNameW(path.c_str(), expected, retval.data(),
        nullptr);
    THROW_LAST_ERROR_IF(actual > expected);

    // Remove the superfluous null at the end.
    retval.resize(actual);

    return retval;
}
