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


#include "eckit/geo/grid/unstructured/ICON.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::grid::unstructured {


ICON::ICON(const uid_type& uid, const std::string& arrangement, const std::string& name) :
    name_(name), arrangement_(arrangement_from_string(arrangement)) {
    reset_uid(uid);
}


ICON::ICON(const Spec& spec) : ICON(spec.get_string("uid"), spec.get_string("arrangement"), spec.get_string("name")) {}


ICON::ICON(uid_type uid) : ICON(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


ICON::ICON(const std::string& name, Arrangement a) :
    ICON(*std::unique_ptr<Spec>(
        GridFactory::make_spec(spec::Custom({{"grid", name + '_' + arrangement_to_string(a)}})))) {}


std::string ICON::arrangement() const {
    return arrangement_to_string(arrangement_);
}


const std::vector<double>& ICON::longitudes() const {
    return cache::LatitudeLongitude::get(uid()).longitude();
}


const std::vector<double>& ICON::latitudes() const {
    return cache::LatitudeLongitude::get(uid()).latitude();
}


Grid::Spec* ICON::spec(const std::string& name) {
    return GridSpecByUID::instance().get(name).spec();
}


Arrangement ICON::arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::ICON_C
           : str == "V" ? Arrangement::ICON_V
           : str == "E" ? Arrangement::ICON_E
                        : throw SeriousBug("ICON: unsupported arrangement '" + str + "'");
}


std::string ICON::arrangement_to_string(Arrangement a) {
    return a == Arrangement::ICON_C   ? "C"
           : a == Arrangement::ICON_V ? "V"
           : a == Arrangement::ICON_E
               ? "E"
               : throw SeriousBug("ICON: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


void ICON::fill_spec(spec::Custom& custom) const {
    ASSERT(!name_.empty());
    custom.set("grid", name_);  // only T-arrangement is currently catalogued

    if (auto _uid = uid(); !GridSpecByUID::instance().exists(_uid)) {
        custom.set("uid", _uid);
    }
}


const std::string& ICON::type() const {
    static const std::string type{"ICON"};
    return type;
}


std::vector<size_t> ICON::shape() const {
    return catalog().get_unsigned_vector("shape");
}


Grid::BoundingBox* ICON::calculate_bbox() const {
    if (const std::string BOUNDING_BOX = "bounding_box"; catalog().has(BOUNDING_BOX)) {
        const auto bbox = catalog().get_double_vector(BOUNDING_BOX);
        ASSERT(bbox.size() == 4);

        return new BoundingBox{bbox[0], bbox[1], bbox[2], bbox[3]};
    }

    return Unstructured::calculate_bbox();
}


static const GridRegisterType<ICON> GRIDTYPE("ICON");


}  // namespace eckit::geo::grid::unstructured
