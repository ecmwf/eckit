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


namespace grit {


using Point2 = std::array<double, 2>;
struct Point3 : std::array<double, 3> {
    static double distance2(const Point3& a, const Point3& b) { return 0.; }
};


}  // namespace grit
