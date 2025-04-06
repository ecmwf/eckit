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


#include "eckit/geo/Order.h"

#include <map>
#include <numeric>
#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


std::map<std::string, size_t> ORDERING_INDEX;
std::map<size_t, std::string> ORDERING_NAME;


}  // namespace


std::string Order::className() {
    static const std::string ordering = "ordering";
    return ordering;
}


Order::value_type Order::make_ordering_from_spec(const Spec&) {
    NOTIMP;

    // int key = (spec.get_bool("scan_i_plus", true) ? 0 : 1) + (spec.get_bool("scan_j_plus", false) ? 1 << 1 : 0) +
    //           (spec.get_bool("scan_ij", true) ? 0 : 1 << 2) + (spec.get_bool("scan_alternating", false) ? 1 << 3 :
    //           0);
    // return static_cast<value_type>(key);
}


Reordering Order::no_reorder(size_t size) {
    Reordering ren(size);
    std::iota(ren.begin(), ren.end(), 0);
    return ren;
}


void Order::register_ordering(const std::string& name) {
    lock_type lock;

    auto index = ORDERING_INDEX.size();

    ASSERT(ORDERING_INDEX.emplace(name, index).second);
    ASSERT(ORDERING_NAME.emplace(index, name).second);
}


const Order* ReorderFactory::make_from_string(const std::string&) {
    // FIXME
    NOTIMP;
}


ReorderFactory& ReorderFactory::instance() {
    static ReorderFactory obj;
    return obj;
}


const Order* ReorderFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return ReorderFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Reorder: cannot build ordering without 'type', choices are: ");
    throw exception::SpecError("Reorder: cannot build ordering without 'type'", Here());
}


Spec* ReorderFactory::make_spec_(const Spec& spec) const {
    lock_type lock;

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to orderingspec)

    // cfg->push_back(new spec::Custom{{"type", ""}});

    return cfg;
}


std::ostream& ReorderFactory::list_(std::ostream& out) const {
    lock_type lock;

    out << ReorderFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
