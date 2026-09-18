// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date   May 2018

#ifndef eckit_io_AutoCloser_h
#define eckit_io_AutoCloser_h

#include <utility>

namespace eckit {

template <typename T>
class AutoCloser {

    T& obj_;

public:  // methods

    AutoCloser(T& obj) : obj_(obj) {}

    /// Assume that close() can throw
    /// Otherwise we could test the interface with:
    /// \code{.cpp}
    /// ~AutoCloser() noexcept(noexcept(std::declval<T>().close()))
    /// \endcode
    /// but Intel compiler 17 on Cray XC40 has trouble with it
    ~AutoCloser() noexcept(false) { obj_.close(); }
};

template <typename T>
AutoCloser<T> closer(T& obj) {
    return AutoCloser<T>(obj);
}


}  // namespace eckit

#endif
