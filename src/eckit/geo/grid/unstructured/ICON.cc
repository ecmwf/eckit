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


namespace eckit::geo::grid::unstructured {


static Arrangement arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::ICON_C
           : str == "V" ? Arrangement::ICON_V
           : str == "E" ? Arrangement::ICON_E
                        : throw SeriousBug("ICON: unsupported arrangement '" + str + "'");
}


static std::string arrangement_to_string(Arrangement a) {
    return a == Arrangement::ICON_C   ? "C"
           : a == Arrangement::ICON_V ? "V"
           : a == Arrangement::ICON_E
               ? "E"
               : throw SeriousBug("ICON: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


ICON::ICON(const Spec& spec) :
    Unstructured(spec.get_string("uid"), spec.get_string("name"),
                 arrangement_to_string(arrangement_from_string(spec.get_string("arrangement")))) {}


ICON::ICON(const uid_type& uid) : ICON(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


ICON::ICON(const std::string& name, Arrangement a) :
    ICON(*[](const std::string& grid) {
        return std::unique_ptr<const spec::Spec>(GridSpecByName::instance().match(grid).spec(grid));
    }(name + "_" + arrangement_to_string(a))) {}


const std::string& ICON::type() const {
    static const std::string type{"ICON"};
    return type;
}


std::vector<size_t> ICON::shape() const {
    return catalog().get_unsigned_vector("shape");
}


Grid::Spec* ICON::spec(const std::string& str) {
    return is_uid(str) ? GridSpecByUID::instance().get(str).spec() : GridSpecByName::instance().get(str).spec(str);
}


static const GridRegisterType<ICON> GRIDTYPE("ICON");


}  // namespace eckit::geo::grid::unstructured
