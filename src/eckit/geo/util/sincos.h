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

#include <array>
#include <cmath>


namespace eckit::geo::util {


struct sincos_t final : std::array<double, 2> {
    explicit sincos_t(value_type r) : array{std::sin(r), std::cos(r)} {}

    const value_type& sin = array::operator[](0);
    const value_type& cos = array::operator[](1);
};


}  // namespace eckit::geo::util
