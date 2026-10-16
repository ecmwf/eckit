// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date   May 2018

#ifndef eckit_io_AutoCloser_h
#define eckit_io_AutoCloser_h

#include <exception>
#include <ostream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

namespace eckit {

/// Calls `close()` on an object when the scope is left.
///
/// A failure of close() reaches the caller when the scope is left normally. While an exception is propagating on the
/// thread it is logged instead, since a destructor must not throw then: the exception in flight wins. That includes a
/// guard created and destroyed inside a destructor run by unwinding, which therefore never ends in std::terminate.
/// Inside a catch block nothing is propagating, so a failure still reaches the caller.
///
/// close() is not retried, so it has to release the resource even when it throws.
///
/// Not thread safe: a guard is a local of one thread. Whether close() may run alongside other calls is up to the
/// object.
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
    ~AutoCloser() noexcept(false) {  // NOLINT
        if (!Exception::throwing()) {
            obj_.close();
            return;
        }

        try {
            obj_.close();
        }
        catch (const std::exception& error) {
            Log::error() << "** Failed to close while an exception is propagating, ignored: " << error.what()
                         << std::endl;
        }
        catch (...) {
            Log::error() << "** Failed to close while an exception is propagating, ignored" << std::endl;
        }
    }
};

template <typename T>
AutoCloser<T> closer(T& obj) {
    return AutoCloser<T>(obj);
}


}  // namespace eckit

#endif
