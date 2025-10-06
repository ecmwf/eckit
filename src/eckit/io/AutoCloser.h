/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
