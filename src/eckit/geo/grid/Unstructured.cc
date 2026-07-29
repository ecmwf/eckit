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


#include "eckit/geo/grid/Unstructured.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/Grid.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
}


namespace eckit::geo::grid {


static Grid::uid_type uid_from_cached_ll(cache::LatitudeLongitude&& ll) {
    auto uid = grid::Unstructured::uid_from_latlons(ll.latitude(), ll.longitude());
    (void)cache::LatitudeLongitude::set(uid, std::move(ll));
    return uid;
}


Unstructured::Unstructured(const Spec& spec) :
    Unstructured(spec.has("uid")
                     ? spec.get_string("uid")
                     : uid_from_cached_ll({spec.get_double_vector("latitudes"), spec.get_double_vector("longitudes")}),
                 spec.get_string("name", [&spec]() {
                     std::string grid;
                     return spec.get("grid", grid) && is_uid(grid) ? "" : grid;
                 }())) {}


Unstructured::Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes,
                           const std::string& name) :
    Unstructured(uid_from_cached_ll({latitudes, longitudes}), name) {}


Unstructured::Unstructured(const uid_type& uid, const std::string& name) {
    ASSERT(is_uid(uid));
    reset_uid(uid);
    name_ = name;

    using custom_map_t = spec::Custom::container_type;

    struct Generator final : GridSpecByUID::concrete_generator_t, GridSpecByName::concrete_generator_t {
        Generator(const custom_map_t& _custom) : custom(_custom) {}
        spec::Spec* spec() const override { return new spec::Custom(custom); }
        spec::Spec* spec(const std::string&) const override { return new spec::Custom(custom); }
        custom_map_t custom;
    };

    custom_map_t custom_map{{"cached_path", record().to_cached_path()}, {"type", type()}, {"uid", uid}};
    if (!name.empty()) {
        custom_map["name"] = name;
    }


    // register uid and name in memory
    if (GridSpecByUID::instance().exists(uid)) {
        GridSpecByUID::instance().unregist(uid);
    }
    GridSpecByUID::regist(uid, new Generator(custom_map));
    ASSERT(GridSpecByUID::instance().exists(uid));

    if (!name.empty()) {
        if (GridSpecByName::instance().exists(name)) {
            GridSpecByName::instance().unregist(name);
        }
        GridSpecByName::regist(name, new Generator(custom_map));
        ASSERT(GridSpecByName::instance().exists(name));
    }


    // register on disk
    cache::Grid::save(uid, spec::Custom{custom_map});
}


Unstructured::Unstructured(const uid_type& uid, const std::string& name, const std::string& arrangement) :
    name_(name), arrangement_(arrangement) {
    reset_uid(uid);
}


std::vector<size_t> Unstructured::shape() const {
    return {record().size()};
}


Grid::iterator Unstructured::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, record().longitude(), record().latitude())};
}


Grid::iterator Unstructured::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Grid::BoundingBox* Unstructured::calculate_bbox() const {
    if (const std::string BOUNDING_BOX = "bounding_box"; catalog().has(BOUNDING_BOX)) {
        const auto bbox = catalog().get_double_vector(BOUNDING_BOX);
        ASSERT(bbox.size() == 4);

        return new BoundingBox{bbox[0], bbox[1], bbox[2], bbox[3]};
    }

    return new BoundingBox;
}


std::vector<Point> Unstructured::to_points() const {
    const auto& lons = record().longitude();
    const auto& lats = record().latitude();

    std::vector<Point> pts;
    pts.reserve(size());

    for (size_t i = 0; i < lons.size(); ++i) {
        pts.emplace_back(PointLonLat{lons[i], lats[i]});
    }

    return pts;
}


std::pair<std::vector<double>, std::vector<double>> Unstructured::to_latlons() const {
    return {record().latitude(), record().longitude()};
}


Grid::uid_type Unstructured::calculate_uid() const {
    return uid_from_latlons(record().latitude(), record().longitude());
}


Grid::uid_type Unstructured::uid_from_latlons(const std::vector<double>& lat, const std::vector<double>& lon) {
    ASSERT(lat.size() == lon.size());

    MD5 hash;
    util::hash_vector_double(hash, lat);
    util::hash_vector_double(hash, lon);

    auto d = hash.digest();
    ASSERT(is_uid(d));
    return {d};
}


void Unstructured::fill_spec(spec::Custom& custom) const {
    if (!name_.empty()) {
        auto grid = name_ + (name_.empty() || arrangement_.empty() ? "" : "_" + arrangement_);
        custom.set("grid", grid);

        if (auto _uid = uid(); !GridSpecByName::instance().exists(grid) ||
                               GridSpecByName::instance().match(grid).spec(grid)->get_string("uid") != _uid) {
            custom.set("uid", _uid);
        }
        return;
    }

    // NOTE: every unstructured grid is known by uid
    std::unique_ptr<spec::Custom> ptr(dynamic_cast<spec::Custom*>(GridSpecByUID::instance().get(uid()).spec()));
    ASSERT(ptr);

    auto custom_map = ptr->container();
    custom_map.erase("cached_path");
    custom.set(custom_map);
}


const std::string& Unstructured::type() const {
    static const std::string type{"unstructured_ll"};
    return type;
}


void Unstructured::cache() const {
    (void)record();
}


const cache::LatitudeLongitude& Unstructured::record() const {
    return cache::LatitudeLongitude::get(uid());
}


static const GridRegisterType<Unstructured> GRIDTYPE1("unstructured");
static const GridRegisterType<Unstructured> GRIDTYPE2("unstructured_ll");


}  // namespace eckit::geo::grid
