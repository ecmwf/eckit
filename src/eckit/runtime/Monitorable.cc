/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Monitorable.h"
#include "eckit/log/JSON.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Monitorable::Monitorable() :
    ClassExtent<Monitorable>(this) {}

Monitorable::~Monitorable() {}

void Monitorable::status(std::ostream& s) const {
    print(s);
    s << std::endl;
}

void Monitorable::print(std::ostream& s) const {
    s << "No print method" << std::endl;
}

void Monitorable::allStatuses(std::ostream& s) {
    callAll(&Monitorable::status, s);
}

void Monitorable::allJSON(JSON& s) {
    s.startList();
    callAll(&Monitorable::json, s);
    s.endList();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
