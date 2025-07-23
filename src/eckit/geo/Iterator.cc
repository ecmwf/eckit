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


#include "eckit/geo/Iterator.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


IteratorFactory& IteratorFactory::instance() {
    static IteratorFactory INSTANCE;
    return INSTANCE;
}


Iterator* IteratorFactory::build_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return IteratorFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Iterator: cannot build iterator without 'type', choices are: ");
    throw exception::SpecError("Iterator: cannot build iterator without 'type'", Here());
}


Spec* IteratorFactory::make_spec_(const Spec& spec) const {
    lock_type lock;

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to spec)

    auto back = std::make_unique<spec::Custom>();
    ASSERT(back);

    back->set("type", cfg->has("pl") ? "reduced" : "regular");

    return cfg;
}


std::ostream& IteratorFactory::list_(std::ostream& out) const {
    lock_type lock;

    out << IteratorFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
