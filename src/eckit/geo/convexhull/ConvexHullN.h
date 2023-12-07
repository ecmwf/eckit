/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/convexhull/Qhull.h"

#include <array>
#include <vector>


namespace eckit::geo {
class Grid;
}  // namespace eckit::geo


namespace eckit::geo::convexhull {


class ConvexHullN : public Qhull {
public:
    template <size_t _Size>
    explicit ConvexHullN(const std::vector<std::array<double, _Size>>&);

    explicit ConvexHullN(const std::vector<std::vector<double>>&);

private:
    explicit ConvexHullN(size_t N, const std::vector<double>&);
};


}  // namespace eckit::geo::convexhull
