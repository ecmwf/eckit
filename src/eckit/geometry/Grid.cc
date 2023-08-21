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


#include "eckit/geometry/Grid.h"

#include <memory>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/GridConfig.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


Grid::Grid(const Configuration& config) :
    bbox_(config) {}


Grid::Grid(const area::BoundingBox& bbox) :
    bbox_(bbox) {}


const area::BoundingBox& Grid::boundingBox() const {
    throw NotImplemented("Grid::boundingBox", Here());
}


void Grid::boundingBox(const area::BoundingBox& bbox) {
    bbox_ = bbox;
}


size_t Grid::size() const {
    throw NotImplemented("Grid::size", Here());
}


bool Grid::includesNorthPole() const {
    throw NotImplemented("Grid::includesNorthPole", Here());
}


bool Grid::includesSouthPole() const {
    throw NotImplemented("Grid::includesSouthPole", Here());
}


bool Grid::isPeriodicWestEast() const {
    throw NotImplemented("Grid::isPeriodicWestEast", Here());
}


std::vector<Point> Grid::to_points() const {
    std::vector<Point> points;
    points.reserve(size());

    for (const auto& p : *this) {
        const auto& q = std::get<PointLonLat>(p);
        points.push_back(p);
    }

    return points;
}


std::pair<std::vector<double>, std::vector<double>> Grid::to_latlon() const {
    throw NotImplemented("Grid::to_latlon");
}


Ordering Grid::order() const {
    throw NotImplemented("Grid::order");
}


Renumber Grid::reorder(const PointLonLat&) const {
    throw NotImplemented("Grid::reorder");
}


GridFactory& GridFactory::instance() {
    static GridFactory obj;
    return obj;
}


const Grid* GridFactory::build_(const Configuration& config) const {
    AutoLock<Mutex> lock(mutex_);

    GridConfig::instance();

    if (std::string uid; config.get("uid", uid)) {
        std::unique_ptr<Configuration> cfg(GridConfigurationUID::instance().get(uid).config());
        return build(*cfg);
    }

    if (std::string name; config.get("name", name)) {
        std::unique_ptr<Configuration> cfg(GridConfigurationName::instance().match(name).config(name));
        return build(*cfg);
    }

    if (std::string type; config.get("type", type)) {
        return GridFactoryType::instance().get(type).create(config);
    }

    list(Log::error() << "Grid: cannot build grid, choices are: ");
    throw SeriousBug("Grid: cannot build grid");
}


void GridFactory::list_(std::ostream& out) const {
    AutoLock<Mutex> lock(mutex_);

    GridConfig::instance();

    out << GridConfigurationUID::instance() << std::endl;
    out << GridConfigurationName::instance() << std::endl;
    out << GridFactoryType::instance() << std::endl;
}


}  // namespace eckit::geometry
