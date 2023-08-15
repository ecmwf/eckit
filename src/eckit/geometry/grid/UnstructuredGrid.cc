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

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/iterator/ListI.h"


namespace eckit::geometry::grid {


UnstructuredGrid::UnstructuredGrid(const Configuration& config) :
    UnstructuredGrid(std::make_pair(config.getDoubleVector("longitudes"), config.getDoubleVector("latitudes"))) {
}


UnstructuredGrid::UnstructuredGrid(const Grid& grid) :
    UnstructuredGrid(grid.to_latlon()) {}


UnstructuredGrid::UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes) :
    Grid(area::BoundingBox::make_global_prime()), latitudes_(latitudes), longitudes_(longitudes) {
    ASSERT(!latitudes_.empty());
    ASSERT(latitudes_.size() == longitudes_.size());
}


UnstructuredGrid::UnstructuredGrid(std::pair<std::vector<double>, std::vector<double>>&& latlon) :
    Grid(area::BoundingBox::make_global_prime()), latitudes_(std::move(latlon.first)), longitudes_(std::move(latlon.second)) {
}


Grid::iterator UnstructuredGrid::cbegin() const {
    return iterator{new geometry::iterator::ListI(*this)};
}


Grid::iterator UnstructuredGrid::cend() const {
    return iterator{new geometry::iterator::ListI(*this, size())};
}


UnstructuredGrid::~UnstructuredGrid() = default;


size_t UnstructuredGrid::size() const {
    return latitudes_.size();
}


std::vector<Point> UnstructuredGrid::to_points() const {
    std::vector<Point> points;
    points.reserve(size());

    for (auto lat = latitudes_.begin(), lon = longitudes_.begin(); lat != latitudes_.end(); ++lat, ++lon) {
        points.emplace_back(PointLonLat{*lon, *lat});
    }

    return points;
}


std::pair<std::vector<double>, std::vector<double>> UnstructuredGrid::to_latlon() const {
    return {latitudes_, longitudes_};
}


static const GridRegisterType<UnstructuredGrid> unstructured_grid("unstructured_grid");


}  // namespace eckit::geometry::grid
