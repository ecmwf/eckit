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


#include "eckit/geo/grid/unstructured/UnstructuredFromGrid.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Unstructured.h"


namespace eckit::geo::grid::unstructured {


namespace {


std::vector<double> points_lat(const std::vector<Point>& points) {
    std::vector<double> l(points.size());
    std::transform(points.begin(), points.end(), l.begin(), [](const Point& p) {
        return std::get<PointLonLat>(p).lat;
    });
    return l;
}


std::vector<double> points_lon(const std::vector<Point>& points) {
    std::vector<double> l(points.size());
    std::transform(points.begin(), points.end(), l.begin(), [](const Point& p) {
        return std::get<PointLonLat>(p).lon;
    });
    return l;
}


}  // namespace


UnstructuredFromGrid::UnstructuredFromGrid(const std::vector<Point>& points) :
    Unstructured(area::BoundingBox::make_global_prime()),
    latitudes_(points_lat(points)),
    longitudes_(points_lon(points)) {
    ASSERT(size() > 0);
}


Grid::iterator UnstructuredFromGrid::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0)};
}


Grid::iterator UnstructuredFromGrid::cend() const {
    return iterator{new geo::iterator::Unstructured(*this, size())};
}


const area::BoundingBox& UnstructuredFromGrid::boundingBox() const {
    return Unstructured::boundingBox();
}


std::vector<Point> UnstructuredFromGrid::to_points() const {
    std::vector<Point> p;
    for (size_t i = 0; i < size(); ++i) {
        p[i] = PointLonLat{longitudes_[i], latitudes_[i]};
    }
    return p;
}


Spec* UnstructuredFromGrid::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


Grid::uid_t UnstructuredFromGrid::uid() const {
    NOTIMP;
}


}  // namespace eckit::geo::grid::unstructured
