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


#include "eckit/geo/grid/ORCA.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
}


namespace eckit::geo::grid {


ORCA::ORCA(const Spec& spec) :
    name_(spec.get_string("name")), arrangement_(arrangement_from_string(spec.get_string("arrangement"))) {
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


Grid::iterator ORCA::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, record().longitude(), record().latitude())};
}


Grid::iterator ORCA::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Point ORCA::first_point() const {
    ASSERT(!empty());
    return PointLonLat{record().longitude().front(), record().latitude().front()};
}


Point ORCA::last_point() const {
    ASSERT(!empty());
    return PointLonLat{record().longitude().back(), record().latitude().back()};
}


std::vector<Point> ORCA::to_points() const {
    const auto& lon = record().longitude();
    const auto& lat = record().latitude();

    std::vector<Point> p;
    p.reserve(size());

    for (size_t i = 0; i < size(); ++i) {
        p.emplace_back(PointLonLat{lon[i], lat[i]});
    }
    return p;
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
    return GridSpecByUID::instance().get(uid).spec();
}


Arrangement ORCA::arrangement_from_string(const std::string& str) {
    return str == "F"   ? Arrangement::ORCA_F
           : str == "T" ? Arrangement::ORCA_T
           : str == "U" ? Arrangement::ORCA_U
           : str == "V" ? Arrangement::ORCA_V
           : str == "W" ? Arrangement::ORCA_W
                        : throw SeriousBug("ORCA: unsupported arrangement '" + str + "'");
}


std::string ORCA::arrangement_to_string(Arrangement a) {
    return a == Arrangement::ORCA_F   ? "F"
           : a == Arrangement::ORCA_T ? "T"
           : a == Arrangement::ORCA_U ? "U"
           : a == Arrangement::ORCA_V ? "V"
           : a == Arrangement::ORCA_W
               ? "W"
               : throw SeriousBug("ORCA: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


void ORCA::fill_spec(spec::Custom& custom) const {
    auto grid = name_ + "_" + arrangement_to_string(arrangement_);
    custom.set("grid", grid);

    if (auto _uid = uid(); !GridSpecByName::instance().exists(grid) ||
                           GridSpecByName::instance().match(grid).spec(grid)->get_string("uid") != _uid) {
        custom.set("uid", _uid);
    }
}


const std::string& ORCA::type() const {
    static const std::string type{"ORCA"};
    return type;
}


std::vector<size_t> ORCA::shape() const {
    return catalog().get_unsigned_vector("shape");
}


Grid::BoundingBox* ORCA::calculate_bbox() const {
    return new BoundingBox;
}


static const GridRegisterType<ORCA> GRIDTYPE("ORCA");


}  // namespace eckit::geo::grid
