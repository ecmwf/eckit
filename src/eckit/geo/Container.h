// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>


namespace eckit::geo {


template <typename T>
class Container {
protected:

    Container() = default;

public:

    virtual ~Container() = default;

    Container(const Container&) = delete;
    Container(Container&&)      = delete;

    Container& operator=(const Container&) = delete;
    Container& operator=(Container&&)      = delete;

    virtual T get(size_t index) const = 0;
    virtual size_t size() const       = 0;
};


}  // namespace eckit::geo
