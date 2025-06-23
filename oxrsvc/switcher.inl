// <copyright file="switcher.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * switcher::scan
 */
template<class TIterator> bool switcher::scan(_In_ const TIterator begin,
        _In_ const TIterator end, _Out_ std::size_t& length) {
    length = 0;

    auto required = 4;
    for (auto it = begin; it != end; ++it, ++length) {
        if (*it == 0) {
            // Found one, decrement the counter. If the counter reached
            // zero, we found the end of the request.
            if (--required == 0) {
                ::OutputDebugString(_T("End of request found"));
                return true;
            }

        } else {
            // If we have any non-zero, start over.
            required = 4;
        }
    }

    return false;
}
