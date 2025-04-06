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


#include "eckit/geo/order/ReducedScan.h"

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::order {


static ReorderRegisterType<ReducedScan> ORDERING("reduced-scan");


const std::string& ReducedScan::type() const {
    static const std::string type{"reduced-scan"};
    return type;
}


Reordering ReducedScan::reorder(value_type from, value_type to) const {
    // FIXME implement this

    throw exception::ReorderError("ReducedScan::reorder(" + from + ", " + to + ")", Here());
}


}  // namespace eckit::geo::order
