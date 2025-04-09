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
#include "eckit/parser/YAMLParser.h"


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


std::map<std::string, size_t> ORDER_INDEX;
std::map<size_t, std::string> ORDER_NAME;


}  // namespace


const Spec& Order::spec() const {
    if (!spec_) {
        spec_ = std::make_shared<spec::Custom>();
        ASSERT(spec_);

        auto& custom = *spec_;
        fill_spec(custom);
    }

    return *spec_;
}


std::string Order::className() {
    static const std::string order = "order";
    return order;
}


Order::value_type Order::make_order_from_spec(const Spec& spec) {
    return std::unique_ptr<const Order>(OrderFactory::build(spec))->order();
}


Reordering Order::no_reorder(size_t size) {
    Reordering ren(size);
    std::iota(ren.begin(), ren.end(), 0);
    return ren;
}


void Order::register_ordering(const std::string& name) {
    lock_type lock;

    auto index = ORDER_INDEX.size();
    ASSERT(ORDER_INDEX.try_emplace(name, index).second == ORDER_NAME.try_emplace(index, name).second);
}


const Order* OrderFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


OrderFactory& OrderFactory::instance() {
    static OrderFactory obj;
    return obj;
}


const Order* OrderFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return OrderFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Order: cannot build order without 'type', choices are: ");
    throw exception::SpecError("Order: cannot build order without 'type'", Here());
}


Spec* OrderFactory::make_spec_(const Spec& spec) const {
    lock_type lock;

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to orderspec)

    cfg->push_back(new spec::Custom{{"type", "scan"}});

    return cfg;
}


std::ostream& OrderFactory::list_(std::ostream& out) const {
    lock_type lock;

    out << OrderFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
