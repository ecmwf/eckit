/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
