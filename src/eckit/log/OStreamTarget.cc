/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"

#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

OStreamTarget::OStreamTarget(std::ostream& out) :
    out_(out) {}

OStreamTarget::~OStreamTarget() {}

void OStreamTarget::write(const char* start, const char* end) {
    out_.write(start, end - start);
}
void OStreamTarget::flush() {
    out_.flush();
}

void OStreamTarget::print(std::ostream& s) const {
    s << "OStreamTarget()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
