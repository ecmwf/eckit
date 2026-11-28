// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/Unstructured.h"

#include <utility>

#include "eckit/cache/FileCache.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Grid.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"


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
                 spec.get_string("name", spec.get_string("cache_as", "")), bounding_box_from_spec(spec),
                 projection_from_spec(spec), spec.get_bool("duplicates_allowed", false)) {}


Unstructured::Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes,
                           const std::string& name, bool duplicates_allowed) :
    Unstructured(uid_from_cached_ll({latitudes, longitudes}), name, nullptr, nullptr, duplicates_allowed) {}


Unstructured::Unstructured(const uid_type& _uid, const std::string& name, std::unique_ptr<BoundingBox> bbox,
                           std::unique_ptr<const Projection> p, bool duplicates_allowed) :
    Grid(std::move(bbox), std::move(p)), duplicates_allowed_(duplicates_allowed) {
    ASSERT(is_uid(_uid));
    const auto uid = StringTools::lower(_uid);
    reset_uid(uid);
    name_ = name;

    spec::Custom::container_type custom_map{{"type", type()}, {"uid", uid}};
    if (duplicates_allowed_) {
        custom_map["duplicates_allowed"] = true;
    }

    // the coordinates file lets other processes use the grid by uid; with nowhere to write, only if found (else memory)
    if (const auto path =
            LibEcKitGeo::cacheWritablePaths().empty() ? record().find_cached_path() : record().to_cached_path()) {
        custom_map["cached_path"] = *path;
    }
    if (!name.empty()) {
        custom_map["name"] = name;
    }

    // the shared grids loaded first (not to replace this grid's registration later), and a name of its own, not of
    // another grid (or matching a grid name pattern)
    const auto& names = GridSpecByName::instance();
    if (!name.empty() && names.matches(name) && !GridSpecByName::has_name(name)) {
        throw exception::SpecError("Unstructured: name '" + name + "' is of another grid", Here());
    }

    const spec::Custom custom(custom_map);

    GridSpecByUID::replace(uid, custom);
    if (!name.empty()) {
        GridSpecByName::replace(name, custom);
    }

    cache::Grid::save(uid, custom);
}


Unstructured::Unstructured(const uid_type& uid, const std::string& name, const std::string& arrangement,
                           std::unique_ptr<BoundingBox> bbox, std::unique_ptr<const Projection> p) :
    Grid(std::move(bbox), std::move(p)), name_(name), arrangement_(arrangement) {
    reset_uid(uid);
}


std::vector<size_t> Unstructured::shape() const {
    return {record().size()};
}


Point Unstructured::point(size_t index) const {
    return record().point(index);
}


std::unique_ptr<Grid::Iterator::State> Unstructured::make_state() const {
    return record().make_state();
}


Grid::BoundingBox* Unstructured::calculate_bbox() const {
    // NOTE: a grid declaring its own bounding box (eg. a regional ICON) gets it from its spec, at construction
    return new BoundingBox;
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
    Grid::fill_spec(custom);

    if (!name_.empty()) {
        // the name with the arrangement if known as such (e.g. ORCA2_T), otherwise the name (e.g. of ICON grids)
        auto grid = name_;
        if (const auto with = name_ + "_" + arrangement_; !arrangement_.empty() && GridSpecByName::has_name(with)) {
            grid = with;
        }
        custom.set("grid", grid);

        // the uid, unless the name gives it
        if (std::unique_ptr<const Spec> named(
                GridSpecByName::has_name(grid) ? GridSpecByName::instance().match_spec(grid, grid) : nullptr);
            !named || StringTools::lower(named->get_string("uid")) != uid()) {
            custom.set("uid", uid());
        }
    }
    else {
        // NOTE: every unstructured grid is known by uid
        std::unique_ptr<spec::Custom> ptr(dynamic_cast<spec::Custom*>(GridSpecByUID::instance().get_spec(uid())));
        ASSERT(ptr);

        // duplicates_allowed is of this grid, not of the latest registration of the uid (same points)
        auto custom_map = ptr->container();
        custom_map.erase("cached_path");
        custom_map.erase("duplicates_allowed");
        custom.set(custom_map);
    }

    if (duplicates_allowed_) {
        custom.set("duplicates_allowed", true);
    }
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
