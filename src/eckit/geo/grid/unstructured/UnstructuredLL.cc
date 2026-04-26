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


#include "eckit/geo/grid/unstructured/UnstructuredLL.h"

#include "eckit/utils/MD5.h"


namespace eckit::geo::grid::unstructured {


Grid::uid_type UnstructuredLL::calculate_uid() const {
    MD5 hash;
    hash.add(type());
    container()->hash(hash);

    auto d = hash.digest();
    ASSERT(Grid::is_uid(d));
    return {d};
}


Grid::Spec* UnstructuredLL::spec(const std::string& name) {
    return GridSpecByUID::instance().get(name).spec();
}


void UnstructuredLL::fill_spec(spec::Custom& custom) const {
    custom.set("grid", name_);
    custom.set("uid", uid());
}


const std::string& UnstructuredLL::type() const {
    static const std::string type{"unstructured_ll"};
    return type;
}


static const GridRegisterType<UnstructuredLL> GRIDTYPE1("unstructured");
static const GridRegisterType<UnstructuredLL> GRIDTYPE2("unstructured_ll");


}  // namespace eckit::geo::grid::unstructured
