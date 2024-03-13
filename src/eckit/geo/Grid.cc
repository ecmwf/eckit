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


std::string Grid::spec() const {
    spec::Custom gridspec;
    this->spec(gridspec);
    return gridspec.str();
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


Grid* Grid::make_grid_reordered(Ordering) const {
    throw NotImplemented("Grid::grid_reorder");
}


Area* Grid::area() const {
    return new area::BoundingBox(bbox_);
}


Renumber Grid::crop(const Area&) const {
    throw NotImplemented("Grid::crop");
}


Grid* Grid::make_grid_cropped(const Area&) const {
    throw NotImplemented("Grid::grid_crop");
}


area::BoundingBox Grid::boundingBox() const {
    return bbox_;
}


Renumber Grid::crop(const area::BoundingBox& bbox) const {
    return crop(static_cast<const Area&>(bbox));
}


Grid* Grid::make_grid_cropped(const area::BoundingBox& bbox) const {
    return make_grid_cropped(static_cast<const Area&>(bbox));
}


Renumber Grid::no_reorder(size_t size) {
    Renumber ren(size);
    std::iota(ren.begin(), ren.end(), 0);
    return ren;
}


void Grid::spec(spec::Custom&) const {
    throw NotImplemented("Grid::spec");
}


const Grid* GridFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(new spec::Custom(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


GridFactory& GridFactory::instance() {
    static GridFactory obj;
    return obj;
}


const Grid* GridFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return GridFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Grid: cannot build grid without 'type', choices are: ");
    throw SpecNotFound("Grid: cannot build grid without 'type'");
}


Spec* GridFactory::make_spec_(const Spec& spec) const {
    lock_type lock;
    etc::Grid::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to gridspec)

    auto back = std::make_unique<spec::Custom>();
    ASSERT(back);

    if (size_t N = 0; cfg->get("N", N)) {
        back->set("grid", "O" + std::to_string(N));
    }

    if (cfg->has("pl")) {
        back->set("type", "reduced_gg");
    }

    if (std::vector<double> grid; cfg->get("grid", grid) && grid.size() == 2) {
        back->set("type", "regular_ll");
    }

    if (!back->empty()) {
        cfg->push_back(back.release());
    }

    if (std::string grid; cfg->get("grid", grid) && SpecByName::instance().matches(grid)) {
        cfg->push_back(SpecByName::instance().match(grid).spec(grid));
    }

    if (std::string uid; cfg->get("uid", uid)) {
        cfg->push_front(SpecByUID::instance().get(uid).spec());
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
