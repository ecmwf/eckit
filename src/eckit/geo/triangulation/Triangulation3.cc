/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/triangulation/Triangulation3.h"

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/projection/LonLatToXYZ.h"

#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullVertexSet.h"


namespace eckit::geo::triangulation {


Triangulation3::Triangulation3(const Grid& grid) {
    auto [lat, lon] = grid.to_latlon();
    ASSERT(lat.size() == lon.size());

    auto N = static_cast<int>(lon.size());
    std::vector<double> coord(N * 3);

    geo::projection::LonLatToXYZ lonlat2xyz(1.);
    for (size_t i = 0; i < N; ++i) {
        auto p = lonlat2xyz.fwd({lon[i], lat[i]});

        coord[i * 3 + 0] = p.X;
        coord[i * 3 + 1] = p.Y;
        coord[i * 3 + 2] = p.Z;
    }

    auto qh = std::make_unique<orgQhull::Qhull>("", 3, N, coord.data(), "Qt");

    reserve(qh->facetCount());
    for (const auto& facet : qh->facetList()) {
        auto vertices = facet.vertices();
        ASSERT(vertices.size() == 3);
        emplace_back(Triangle{static_cast<size_t>(vertices[0].id()),
                              static_cast<size_t>(vertices[1].id()),
                              static_cast<size_t>(vertices[2].id())});
    }
}


}  // namespace eckit::geo::triangulation
