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


#include "eckit/geometry/grid/UnstructuredGrid.h"

#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/iterator/ListI.h"


namespace eckit::geometry::grid {


static const area::BoundingBox __global;


static std::vector<Point> lonlat_to_points(const std::vector<double> lons, const std::vector<double> lats) {
    auto N = lats.size();
    ASSERT(N == lons.size());

    std::vector<Point> points;
    points.reserve(N);

    for (auto lon = lons.begin(), lat = lats.begin(); lon != lons.end(); ++lon, ++lat) {
        points.emplace_back(PointLonLat{*lon, *lat});
    }

    return points;
}


UnstructuredGrid::UnstructuredGrid(const Configuration& config) :
    UnstructuredGrid(config.getDoubleVector("longitudes"), config.getDoubleVector("latitudes")) {
}


UnstructuredGrid::UnstructuredGrid(const Grid& grid) :
    UnstructuredGrid(grid.to_points()) {}


UnstructuredGrid::UnstructuredGrid(std::vector<Point>&& points) :
    Grid(__global), points_(points) {
}


UnstructuredGrid::UnstructuredGrid(const std::vector<double>& longitudes, const std::vector<double>& latitudes) :
    Grid(__global), points_(lonlat_to_points(longitudes, latitudes)) {
}


Grid::iterator UnstructuredGrid::cbegin() const {
    return iterator{new geometry::iterator::ListI(*this)};
}


Grid::iterator UnstructuredGrid::cend() const {
    return iterator{new geometry::iterator::ListI(*this, size())};
}


UnstructuredGrid::~UnstructuredGrid() = default;


size_t UnstructuredGrid::size() const {
    return points_.size();
}


std::vector<Point> UnstructuredGrid::to_points() const {
    return points_;
}


static const GridRegisterType<UnstructuredGrid> unstructured_grid("unstructured_grid");


}  // namespace eckit::geometry::grid
