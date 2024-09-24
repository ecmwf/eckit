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

#include <utility>
#include <vector>

#include "eckit/geo/Point.h"


namespace eckit::geo {


class Container {
protected:
    Container() = default;

    static std::pair<std::vector<double>, std::vector<double>> to_latlon(const std::vector<Point>&);

public:
    virtual ~Container() = default;

    Container(const Container&) = delete;
    Container(Container&&)      = delete;

    Container& operator=(const Container&) = delete;
    Container& operator=(Container&&)      = delete;

    virtual Point get(size_t index) const = 0;
    virtual size_t size() const           = 0;

    [[nodiscard]] virtual std::vector<Point> to_points() const                                  = 0;
    [[nodiscard]] virtual std::pair<std::vector<double>, std::vector<double>> to_latlon() const = 0;
};


}  // namespace eckit::geo
