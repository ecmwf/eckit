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


#include "eckit/geo/grid/unstructured/FESOM.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid::unstructured {


namespace {


FESOM::Arrangement fesom_arrangement_from_string(const std::string& str) {
    return str == "F"   ? FESOM::Arrangement::Nodes
           : str == "T" ? FESOM::Arrangement::Centroids
                        : throw AssertionFailed("FESOM::Arrangement", Here());
}


std::string fesom_arrangement_to_string(FESOM::Arrangement a) {
    return a == FESOM::Arrangement::Nodes       ? "F"
           : a == FESOM::Arrangement::Centroids ? "T"
                                                : throw AssertionFailed("FESOM::Arrangement", Here());
}


std::string fesom_path(const Spec& spec, Grid::uid_t uid) {
    auto path = spec.get_string("path", LibEcKitGeo::cacheDir() + "/eckit/geo/grid/fesom/" + uid + ".fesom.codec");
    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
}


}  // namespace


FESOM::FESOM(const Spec& spec) :
    Unstructured(spec),
    name_(spec.get_string("fesom_name")),
    uid_(spec.get_string("fesom_uid")),
    arrangement_(fesom_arrangement_from_string(spec.get_string("fesom_arrangement"))),
    path_(fesom_path(spec, uid_)) {}


FESOM::FESOM(uid_t uid) : FESOM(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


#if 0
FESOM::FESOM(std::vector<Point>&& points) : Grid(area::BoundingBox{}), points_(points) {}


Grid::iterator FESOM::cbegin() const {
    return iterator{new geo::iterator::FESOM(*this, 0, points_)};
}


Grid::iterator FESOM::cend() const {
    return iterator{new geo::iterator::FESOM(*this)};
}


Spec* FESOM::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


void FESOM::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", "unstructured");
    custom.set("uid", uid());
}
#endif


}  // namespace eckit::geo::grid::unstructured
