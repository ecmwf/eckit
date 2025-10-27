/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_OnlyMovable_h
#define eckit_OnlyMovable_h


namespace eckit {

/// Inherit from this class to make a NonCopyable class

class OnlyMovable {
protected:

    OnlyMovable() {}
    ~OnlyMovable() {}

    // Copying not allowed

    OnlyMovable(const OnlyMovable&)            = delete;
    OnlyMovable& operator=(const OnlyMovable&) = delete;

    // Moving is allowed

    OnlyMovable(OnlyMovable&&)            = default;
    OnlyMovable& operator=(OnlyMovable&&) = default;
};

}  // namespace eckit

#endif
