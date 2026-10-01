// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_OnlyMovable_h
#define eckit_OnlyMovable_h


namespace eckit {

/// Inherit from this class to make a OnlyMovable class

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
