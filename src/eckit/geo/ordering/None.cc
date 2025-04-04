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


#include "eckit/geo/ordering/None.h"

#include <numeric>


namespace eckit::geo::ordering {


static OrderingRegisterType<None> ORDERING("none");


const std::string& None::type() const {
    static const std::string type{"none"};
    return type;
}


Ordering::ordering_type None::to_type() const {
    return static_cast<Ordering::ordering_type>(0);
}


Ordering::ordering_type None::from_type() const {
    return static_cast<Ordering::ordering_type>(0);
}


Reorder None::reorder() const {
    return {};
}


Reorder None::no_reorder(size_t size) {
    Reorder r(size);
    std::iota(r.begin(), r.end(), 0);
    return r;
}


}  // namespace eckit::geo::ordering
