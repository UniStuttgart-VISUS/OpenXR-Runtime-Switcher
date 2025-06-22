// <copyright file="runtime.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "runtime.h"


/*
 * runtime::from_file
 */
runtime runtime::from_file(_In_ const std::wstring& path,
        _In_opt_z_ const wchar_t *wow_path,
        _In_opt_z_ const wchar_t *name) {
    runtime retval;

    std::ifstream f(path);
    retval._name = check_runtime(nlohmann::json::parse(f));
    // 'path' is valid runtime at this point.

    if (wow_path != nullptr) {
        std::ifstream f(wow_path);
        check_runtime(nlohmann::json::parse(f));
        // 'wow_path' is valid runtime at this point.
        retval._wow_path = wow_path;
    }

    retval._path = path;

    if (name != nullptr) {
        retval._name = name;
    }

    return retval;
}


/*
 * runtime::operator =
 */
runtime& runtime::operator =(_Inout_ runtime&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        this->_name = std::move(rhs._name);
        this->_path = std::move(rhs._path);
        this->_wow_path = std::move(rhs._wow_path);
    }

    return *this;
}


/*
 * runtime::check_runtime
 */
std::wstring runtime::check_runtime(_In_ const nlohmann::json& json) {
    constexpr const char *const error_message = "The specified file does not "
        "contain a valid OpenXR runtime description.";

    const auto rt = json.find("runtime");
    if (rt == json.end()) {
        throw std::invalid_argument(error_message);
    }

    const auto path = rt->find("library_path");
    if (path == rt->end()) {
        throw std::invalid_argument(error_message);
    }

    const auto name = rt->find("name");
    if (name != rt->end()) {
        auto n = name->template get<std::string>();

        std::size_t cnt = 0;
        ::mbstowcs_s(&cnt, nullptr, 0, n.c_str(), n.size());
        std::wstring retval(cnt, L'\0');

        ::mbstowcs_s(&cnt, retval.data(), retval.size(), n.c_str(), _TRUNCATE);
        retval.resize(cnt);

        return retval;

    } else {
        return L"";
    }
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
