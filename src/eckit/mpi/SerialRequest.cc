/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/SerialRequest.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/mpi/SerialData.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

SerialRequest::SerialRequest() :
    request_(-1) {}

SerialRequest::~SerialRequest() {}


void SerialRequest::print(std::ostream& os) const {
    os << "SerialRequest("
       << "tag->" << tag() << ")";
}

int SerialRequest::request() const {
    return request_;
}

//----------------------------------------------------------------------------------------------------------------------

SendRequest::SendRequest(const void* buffer, size_t count, Data::Code type, int tag) :
    buffer_(static_cast<const char*>(buffer), count * dataSize[type]), count_(count), tag_(tag), type_(type) {}

SendRequest::~SendRequest() {}

//----------------------------------------------------------------------------------------------------------------------

ReceiveRequest::ReceiveRequest(void* buffer, size_t count, Data::Code type, int tag) :
    buffer_(buffer), count_(count), tag_(tag), type_(type) {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
