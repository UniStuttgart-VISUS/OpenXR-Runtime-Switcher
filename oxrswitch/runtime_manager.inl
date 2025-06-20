// <copyright file="runtime_manager.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>



/*
 * runtime_manager::get_json_files
 */
template<class TIterator>
void runtime_manager::get_json_files(_In_ const std::wstring& folder,
        _In_ TIterator oit) {
    std::stack<std::wstring> stack;
    stack.push(folder);

    while (!stack.empty()) {
        const auto cur = std::move(stack.top());
        stack.pop();

        WIN32_FIND_DATAW fd;
        const auto query = cur + L"\\*";
        wil::unique_hfind find(::FindFirstFileW(query.c_str(), &fd));
        if (!find) {
            continue;
        }

        do {
            if (::equals(fd.cFileName, L".") || ::equals(fd.cFileName, L"..")) {
                continue;
            }

            const auto path = (cur + L"\\") + fd.cFileName;

            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                stack.push(path);

            } else if (::ends_with(path, L".json", false)) {
                *oit++ = std::move(path);
            }
        } while (::FindNextFile(find.get(), &fd) != 0);
    }
}


/*
 * runtime_manager::get_software_paths
 */
template<class TIterator>
void runtime_manager::get_software_paths(_In_ TIterator oit) {

    // Native software.
    {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE");
        get_software_paths(key, oit);
    }

    // 32-bit software on 64-bit systems.
    try {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE\\"
            L"WOW6432Node");
        get_software_paths(key, oit);
    } catch (...) { /* This is not fatal. */ }
}


/*
 * runtime_manager::get_software_paths
 */
template<class TIterator>
void runtime_manager::get_software_paths(_In_ const wil::unique_hkey& key,
        _In_ TIterator oit) {
    assert(key);

    for (auto it = wil::reg::key_iterator(key.get()),
            end = wil::reg::key_iterator(); it != end; ++it) {
        // Get the "vendor" key below SOFTWARE/WOW6432Node.
        auto v = wil::reg::open_unique_key(key.get(), it->name.c_str());

        for (auto jt = wil::reg::key_iterator(v.get()); jt != end; ++jt) {
            // 'jt' goes over the per-vendor software entries in the vendor key.
            // If the name of this key in combination with the vendor name in
            // 'it' match any of the known runtimes, try to derive the
            // installation location from it.

            for (auto& r : runtime_info::runtimes) {
                if (r.is_match(it->name, jt->name)) {
                    std::wstring path;
                    auto s = wil::reg::open_unique_key(v.get(),
                        jt->name.c_str());
                    if (r.try_get_installation_path(s, path)) {
                        *oit++ = std::move(path);
                    }
                }
            }
        }
    }
}


/*
 * runtime_manager::get_uninstall_paths
 */
template<class TIterator>
void runtime_manager::get_uninstall_paths(_In_ TIterator oit) {

    // Native software.

    {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
        get_uninstall_paths(key, oit);
    }

    // 32-bit software on 64-bit systems.
    try {
        auto key = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, L"SOFTWARE\\"
            L"WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
        get_uninstall_paths(key, oit);
    } catch(...) { /* This is not fatal. */ }
}


/*
 * runtime_manager::get_uninstall_paths
 */
template<class TIterator>
void runtime_manager::get_uninstall_paths(_In_ const wil::unique_hkey& key,
        _In_ TIterator oit) {
    assert(key);
    auto cnt = wil::reg::get_child_key_count(key.get());

    for (auto it = wil::reg::key_iterator(key.get()),
            end = wil::reg::key_iterator(); it != end; ++it) {
        auto k = wil::reg::open_unique_key(key.get(), it->name.c_str());

        for (auto& r : runtime_info::runtimes) {
            std::wstring path;
            if (is_match(k, r, path)) {
                *oit++ = std::move(path);
            }
        }
    }
}
