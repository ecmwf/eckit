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
#include <cctype>
#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/projection/None.h"
#include "eckit/geo/share/Grid.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo {


namespace {


// 128-bit hash = 32 hex characters * 4 bits per character
constexpr size_t DIGEST_LENGTH = 32;
static_assert(DIGEST_LENGTH == MD5_DIGEST_LENGTH * 2, "MD5 digest length mismatch");


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


Grid::Grid(const Spec& spec) :
    bbox_(area::BoundingBox::make_from_spec(spec)), projection_(ProjectionFactory::build(spec)) {}


Grid::Grid(Projection* proj) : projection_(proj == nullptr ? new projection::None : proj) {
    bbox_.reset(area::BoundingBox::make_global_prime().release());
}


const Spec& Grid::spec() const {
    if (!spec_) {
        spec_ = std::make_unique<spec::Custom>();
        ASSERT(spec_);

        auto& custom = *spec_;
        fill_spec(custom);
    }

    return *spec_;
}


bool Grid::empty() const {
    return size() == 0;
}


size_t Grid::size() const {
    NOTIMP;
}


bool Grid::is_uid(const std::string& str) {
    return str.length() == DIGEST_LENGTH &&
           std::all_of(str.begin(), str.end(), [](char c) { return std::isxdigit(static_cast<unsigned char>(c)); });
}


Grid::uid_type Grid::uid() const {
    if (uid_.empty()) {
        const_cast<Grid*>(this)->reset_uid(calculate_uid());
    }

    return uid_;
}


Grid::uid_type Grid::calculate_uid() const {
    auto id = MD5{spec_str()}.digest();
    std::transform(id.begin(), id.end(), id.begin(), [](unsigned char c) { return std::tolower(c); });

    ASSERT(is_uid(id));
    return id;
}


void Grid::reset_uid(uid_type id) {
    if (id.empty()) {
        uid_.clear();
        return;
    }

    ASSERT(is_uid(id));
    std::transform(id.begin(), id.end(), id.begin(), [](unsigned char c) { return std::tolower(c); });

    uid_ = id;
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


Point Grid::first_point() const {
    ASSERT(!empty());
    return to_points().front();
}


Point Grid::last_point() const {
    ASSERT(!empty());
    return to_points().back();
}


std::vector<Point> Grid::to_points() const {
    return {cbegin(), cend()};
}


std::pair<std::vector<double>, std::vector<double> > Grid::to_latlons() const {
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


const Grid::order_type& Grid::order() const {
    NOTIMP;
}


Grid::renumber_type Grid::reorder(const order_type&) const {
    NOTIMP;
}


Grid* Grid::make_grid_reordered(const order_type&) const {
    NOTIMP;
}


const Area& Grid::area() const {
    if (!area_) {
        area_ = std::make_unique<area::BoundingBox>();
        ASSERT(area_);
    }

    return *area_;
}


Grid::renumber_type Grid::crop(const Area&) const {
    NOTIMP;
}


const Projection& Grid::projection() const {
    if (!projection_) {
        projection_ = std::make_unique<projection::None>();
        ASSERT(projection_);
    }

    return *projection_;
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


void Grid::fill_spec(spec::Custom& custom) const {
    if (area_) {
        auto area = std::make_unique<spec::Custom>();
        ASSERT(area);

        area_->fill_spec(*area);
        if (!area->empty()) {
            custom.set("area", area.release());
        }
    }

    if (projection_) {
        auto projection = std::make_unique<spec::Custom>();
        ASSERT(projection);

        projection_->fill_spec(*projection);
        if (!projection->empty()) {
            custom.set("projection", projection.release());
        }
    }
}


const Grid* GridFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


GridFactory& GridFactory::instance() {
    static GridFactory INSTANCE;
    return INSTANCE;
}


const Grid* GridFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return GridFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Grid: cannot build grid without 'type', choices are: ");
    throw exception::SpecError("Grid: cannot build grid without 'type'", Here());
}


Spec* GridFactory::make_spec_(const Spec& spec) const {
    lock_type lock;
    share::Grid::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to spec)

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

    if (std::string grid; cfg->get("grid", grid) && GridSpecByName::instance().matches(grid)) {
        cfg->push_back(GridSpecByName::instance().match(grid).spec(grid));
    }

    if (std::string uid; cfg->get("uid", uid) || (cfg->get("grid", uid) && Grid::is_uid(uid))) {
        cfg->push_front(GridSpecByUID::instance().get(uid).spec());
    }

    return cfg;
}


bool Grid::NextIterator::next(Point& point) {
    if (current_ != end_) {
        point = *current_;
        ++current_;
        return true;
    }

    return false;
}


std::ostream& GridFactory::list_(std::ostream& out) const {
    lock_type lock;
    share::Grid::instance();

    out << GridSpecByUID::instance() << std::endl;
    out << GridSpecByName::instance() << std::endl;
    out << GridFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
