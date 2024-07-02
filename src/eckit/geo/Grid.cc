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

#include <algorithm>
#include <numeric>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/etc/Grid.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo {


static util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


Grid::Grid(const Spec& spec) :
    bbox_(area::BoundingBox::make_from_spec(spec)), ordering_(make_ordering_from_spec(spec)) {}


Grid::Grid(Ordering ordering) : ordering_(ordering) {}


Grid::Grid(const area::BoundingBox& bbox, Projection* projection, Ordering ordering) :
    bbox_(new area::BoundingBox(bbox)), projection_(projection), ordering_(ordering) {}


const Spec& Grid::spec() const {
    if (!spec_) {
        spec_ = std::make_unique<spec::Custom>();
        ASSERT(spec_);

        auto& custom = *spec_;
        fill_spec(custom);

        if (std::string name; SpecByName::instance().match(custom, name)) {
            custom.clear();
            custom.set("grid", name);
        }
    }

    return *spec_;
}


size_t Grid::size() const {
    NOTIMP;
}


Grid::uid_t Grid::uid() const {
    return uid_.empty() ? (uid_ = calculate_uid()) : uid_;
}


Grid::uid_t Grid::calculate_uid() const {
    auto id = MD5{spec_str()}.digest();
    ASSERT(id.length() == MD5_DIGEST_LENGTH * 2);
    return id;
}


bool Grid::includesNorthPole() const {
    NOTIMP;
}


bool Grid::includesSouthPole() const {
    NOTIMP;
}


bool Grid::isPeriodicWestEast() const {
    NOTIMP;
}


std::vector<Point> Grid::to_points() const {
    std::vector<Point> points;
    points.reserve(size());

    std::for_each(cbegin(), cend(), [&points](const auto& p) { points.emplace_back(p); });

    return points;
}


std::pair<std::vector<double>, std::vector<double> > Grid::to_latlon() const {
    std::pair<std::vector<double>, std::vector<double> > ll;
    ll.first.reserve(size());
    ll.second.reserve(size());

    std::for_each(cbegin(), cend(), [&ll](const auto& p) {
        auto q = std::get<PointLonLat>(p);
        ll.first.emplace_back(q.lat);
        ll.second.emplace_back(q.lon);
    });

    return ll;
}


Ordering Grid::ordering() const {
    NOTIMP;
}


Renumber Grid::reorder(Ordering) const {
    NOTIMP;
}


Grid* Grid::make_grid_reordered(Ordering) const {
    NOTIMP;
}


const Area& Grid::area() const {
    if (!area_) {
        area_ = std::make_unique<area::BoundingBox>();
        ASSERT(area_);
    }

    return *area_;
}


Renumber Grid::crop(const Area&) const {
    NOTIMP;
}


Grid* Grid::make_grid_cropped(const Area&) const {
    NOTIMP;
}


const area::BoundingBox& Grid::boundingBox() const {
    if (!bbox_) {
        bbox_.reset(calculate_bbox());
        ASSERT(bbox_);
    }

    return *bbox_;
}


area::BoundingBox* Grid::calculate_bbox() const {
    NOTIMP;
}


Renumber Grid::no_reorder(size_t size) {
    Renumber ren(size);
    std::iota(ren.begin(), ren.end(), 0);
    return ren;
}


void Grid::fill_spec(spec::Custom& custom) const {
    if (area_) {
        static const auto AREA_DEFAULT(area::BOUNDING_BOX_DEFAULT.spec_str());

        std::unique_ptr<spec::Custom> area(area_->spec());
        if (area->str() != AREA_DEFAULT) {
            custom.set("area", area.release());
        }
    }

    if (projection_) {
        projection_->fill_spec(custom);
    }
}


const Grid* GridFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
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
    throw SpecNotFound("Grid: cannot build grid without 'type'", Here());
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
