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

#include "eckit/geo/ConvexHull.h"

#include <memory>
#include <string>
#include <vector>

#include "libqhullcpp/Qhull.h"


namespace eckit::geo::util {


class Qhull : public ConvexHull {
public:
    // -- Types

    Qhull(size_t N, const std::vector<double>& coord, const std::string& command);

    // -- Overridden methods

    std::vector<std::vector<double>> list_vertices() const override;
    std::vector<std::vector<size_t>> list_facets() const override;
    std::vector<Triangle> list_triangles() const override;

private:
    // -- Members

    std::unique_ptr<orgQhull::Qhull> qh_;
};


}  // namespace eckit::geo::util
