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


#include "eckit/geo/grid/unstructured/UnstructuredGeneric.h"

#include "eckit/utils/MD5.h"


namespace eckit::geo::grid::unstructured {


Grid::uid_type UnstructuredGeneric::calculate_uid() const {
    MD5 hash;
    hash.add(type());
    container()->hash(hash);

    auto d = hash.digest();
    ASSERT(Grid::is_uid(d));
    return {d};
}


Spec* UnstructuredGeneric::spec(const std::string& name) {
    return GridSpecByUID::instance().get(name).spec();
}


void UnstructuredGeneric::fill_spec(spec::Custom& custom) const {
    custom.set("grid", name_);
    custom.set("uid", uid());
}


const std::string& UnstructuredGeneric::type() const {
    static const std::string type{"unstructured"};
    return type;
}


static const GridRegisterType<UnstructuredGeneric> GRIDTYPE("unstructured");


}  // namespace eckit::geo::grid::unstructured
