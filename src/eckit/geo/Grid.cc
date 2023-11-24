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


#include "eckit/geo/Grid.h"

#include <memory>
#include <numeric>
#include <ostream>

#include "eckit/config/DynamicConfiguration.h"
#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/GridConfig.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geo {


Grid::Grid(const Configuration& config) :
    bbox_(config) {}


Grid::Grid(const area::BoundingBox& bbox) :
    bbox_(bbox) {}


const area::BoundingBox& Grid::boundingBox() const {
    throw NotImplemented("Grid::boundingBox", Here());
}


size_t Grid::size() const {
    throw NotImplemented("Grid::size", Here());
}


Grid::uid_t Grid::uid() const {
    throw NotImplemented("Grid::uid", Here());
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


Renumber Grid::reorder(Ordering) const {
    throw NotImplemented("Grid::reorder");
}


Grid* Grid::grid_reorder(Ordering) const {
    throw NotImplemented("Grid::grid_reorder");
}


Renumber Grid::no_reorder(size_t size) {
    Renumber ren(size);
    std::iota(ren.begin(), ren.end(), 0);
    return ren;
}


GridFactory& GridFactory::instance() {
    static GridFactory obj;
    return obj;
}


const Grid* GridFactory::build_(const Configuration& config) const {
    AutoLock<Mutex> lock(mutex_);

    std::unique_ptr<Configuration> cfg(configure_(config));

    if (std::string type; cfg->get("type", type)) {
        return GridFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Grid: cannot build grid without 'type', choices are: ");
    throw SeriousBug("Grid: cannot build grid without 'type'");
}


Configuration* GridFactory::configure_(const Configuration& config) const {
    AutoLock<Mutex> lock(mutex_);

    GridConfig::instance();

    auto* cfg = new DynamicConfiguration(config);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to gridspec)

    auto* map = new MappedConfiguration;
    ASSERT(map != nullptr);

    if (size_t N = 0; cfg->get("N", N)) {
        map->set("grid", "O" + std::to_string(N));
    }

    if (std::vector<double> grid; cfg->get("grid", grid) && grid.size() == 2) {
        map->set("type", "regular_ll");
    }

    cfg->push_back(map);


    // configurable options

    if (std::string uid; cfg->get("uid", uid)) {
        cfg->push_front(GridConfigurationUID::instance().get(uid).config());
    }
    else if (std::string grid; cfg->get("grid", grid) && GridConfigurationName::instance().matches(grid)) {
        cfg->push_front(GridConfigurationName::instance().match(grid).config(grid));
    }


    // finalise

    return cfg;
}


void GridFactory::list_(std::ostream& out) const {
    AutoLock<Mutex> lock(mutex_);

    GridConfig::instance();

    out << GridConfigurationUID::instance() << std::endl;
    out << GridConfigurationName::instance() << std::endl;
    out << GridFactoryType::instance() << std::endl;
}


}  // namespace eckit::geo
