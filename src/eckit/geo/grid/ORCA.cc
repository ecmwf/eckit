// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/ORCA.h"

#include <memory>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
}


namespace eckit::geo::grid {


ORCA::ORCA(const Spec& spec) :
    Grid(bounding_box_from_spec(spec), ProjectionFactory::build(spec)),
    name_(spec.get_string("name")),
    arrangement_(arrangement_from_string("ORCA", spec.get_string("arrangement"))) {
    reset_uid(spec.get_string("uid"));
}


ORCA::ORCA(uid_type uid) : ORCA(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


ORCA::ORCA(const std::string& name, Arrangement a) :
    ORCA(*std::unique_ptr<Spec>(
        GridFactory::make_spec(spec::Custom({{"grid", name + '_' + arrangement_to_string(a)}})))) {}


std::string ORCA::arrangement() const {
    return arrangement_to_string(arrangement_);
}


const cache::LatitudeLongitude& ORCA::record() const {
    return cache::LatitudeLongitude::get(uid());
}


Grid::uid_type ORCA::calculate_uid() const {
    MD5 hash;
    hash.add(arrangement_to_string(arrangement_));
    util::hash_vector_double(hash, record().latitude());
    util::hash_vector_double(hash, record().longitude());

    auto d = hash.digest();
    ASSERT(Grid::is_uid(d));

    return {d};
}


Point ORCA::point(size_t index) const {
    return record().point(index);
}


Grid::Iterator::State* ORCA::make_state() const {
    return record().make_state();
}


std::pair<std::vector<double>, std::vector<double>> ORCA::to_latlons() const {
    return {record().latitude(), record().longitude()};
}


const Grid::order_type& ORCA::order() const {
    NOTIMP;
}


Grid::renumber_type ORCA::reorder(const order_type& to) const {
    NOTIMP;
}


Grid::Spec* ORCA::spec_from_uid(const uid_type& uid) {
    return GridSpecByUID::instance().get_spec(uid);
}


void ORCA::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    auto grid = name_ + "_" + arrangement_to_string(arrangement_);
    custom.set("grid", grid);

    // the uid, unless the name gives it
    if (std::unique_ptr<const Spec> named(
            GridSpecByName::has_name(grid) ? GridSpecByName::instance().match_spec(grid, grid) : nullptr);
        !named || util::to_lower(named->get_string("uid")) != uid()) {
        custom.set("uid", uid());
    }
}


const std::string& ORCA::type() const {
    static const std::string type{"ORCA"};
    return type;
}


std::vector<size_t> ORCA::shape() const {
    return catalog().get_unsigned_vector("shape");
}


void ORCA::cache() const {
    (void)record();
}


Grid::BoundingBox* ORCA::calculate_bbox() const {
    return new BoundingBox;
}


static const GridRegisterType<ORCA> GRIDTYPE("ORCA");


}  // namespace eckit::geo::grid
