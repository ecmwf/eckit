/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/SerialRequest.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

SerialRequest::SerialRequest() :
    recvbuf_(NULL),
    sendbuf_(NULL),
    count_(0),
    tag_(0),
    type_(Data::MAX_DATA_CODE),
    is_receive_(false)
{
}

void SerialRequest::print(std::ostream& os) const {
    os << "SerialRequest("
       << ")";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
