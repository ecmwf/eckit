// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/mpi/SerialRequest.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/mpi/SerialData.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

SerialRequest::SerialRequest() : request_(-1) {}

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

}  // namespace eckit::mpi
