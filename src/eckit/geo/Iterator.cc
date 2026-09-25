// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Iterator.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"


namespace eckit::geo {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


IteratorFactory& IteratorFactory::instance() {
    static IteratorFactory INSTANCE;
    return INSTANCE;
}


Iterator* IteratorFactory::build_(const Iterator::Spec& spec) const {
    lock_type lock;

    if (std::string type; spec.get("type", type)) {
        return IteratorFactoryType::instance().get(type).create(spec);
    }

    list(Log::error() << "Iterator: cannot build iterator without 'type', choices are: ");
    throw exception::SpecError("Iterator: cannot build iterator without 'type'", Here());
}


std::ostream& IteratorFactory::list_(std::ostream& out) const {
    lock_type lock;

    out << IteratorFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
