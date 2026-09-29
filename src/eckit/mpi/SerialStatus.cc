// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "eckit/mpi/Serial.h"
#include "eckit/mpi/SerialStatus.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

SerialStatus::SerialStatus() : source_(Serial::Constants::anySource()), tag_(Serial::Constants::anyTag()), error_(0) {}

//----------------------------------------------------------------------------------------------------------------------

void SerialStatus::print(std::ostream& os) const {
    os << "SerialStatus("
       << "source=" << source() << ",tag=" << tag() << ",error=" << error() << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi
