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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/etc/Grid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/spec/Valued.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"


namespace eckit::geo {


static util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


Grid::Grid(const Spec& spec) :
    bbox_(spec) {}


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


std::string Grid::spec() const {
    throw NotImplemented("Grid::spec");
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


const Grid* GridFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(new spec::Valued(YAMLParser::decodeString(str)));
    return instance().build_(*spec);
}


GridFactory& GridFactory::instance() {
    static GridFactory obj;
    return obj;
}


const Grid* GridFactory::build_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(generate_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return GridFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Grid: cannot build grid without 'type', choices are: ");
    throw SpecNotFound("Grid: cannot build grid without 'type'");
}


Spec* GridFactory::generate_spec_(const Spec& spec) const {
    lock_type lock;
    etc::Grid::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to gridspec)

    auto* map = new spec::Custom;
    ASSERT(map != nullptr);

    if (size_t N = 0; cfg->get("N", N)) {
        map->set("grid", "O" + std::to_string(N));
    }

    if (cfg->has("pl")) {
        map->set("type", "reduced_gg");
    }

    if (std::vector<double> grid; cfg->get("grid", grid) && grid.size() == 2) {
        map->set("type", "regular_ll");
    }

    cfg->push_back(map);


    // configurable options

    if (std::string uid; cfg->get("uid", uid)) {
        cfg->push_front(SpecByUID::instance().get(uid).spec());
    }
    else if (std::string grid; cfg->get("grid", grid) && SpecByName::instance().matches(grid)) {
        auto* front = SpecByName::instance().match(grid).spec(grid);
        ASSERT(front != nullptr);

        if (std::string user, forced; front->get("type", forced) && cfg->get("type", user) && user != forced) {
            throw BadParameter("Grid: conflicting 'type': '" + user + "' and '" + forced + "'");
        }

        cfg->push_front(front);
    }


    // finalise

    return cfg;
}


void GridFactory::list_(std::ostream& out) const {
    lock_type lock;
    etc::Grid::instance();

    out << SpecByUID::instance() << std::endl;
    out << SpecByName::instance() << std::endl;
    out << GridFactoryType::instance() << std::endl;
}


}  // namespace eckit::geo
