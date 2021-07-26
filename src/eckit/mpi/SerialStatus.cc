/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/mpi/Serial.h"
#include "eckit/mpi/SerialStatus.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

SerialStatus::SerialStatus() :
    source_(Serial::Constants::anySource()), tag_(Serial::Constants::anyTag()), error_(0) {}

//----------------------------------------------------------------------------------------------------------------------

void SerialStatus::print(std::ostream& os) const {
    os << "SerialStatus("
       << "source=" << source() << ",tag=" << tag() << ",error=" << error() << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
