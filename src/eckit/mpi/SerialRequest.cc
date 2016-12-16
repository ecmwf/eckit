/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/SerialData.h"
#include "eckit/mpi/SerialRequest.h"
#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

SerialRequest::SerialRequest() :
    request_(-1) {
}

SerialRequest::~SerialRequest() {
}


void SerialRequest::print(std::ostream& os) const {
    os << "SerialRequest("
       << ")";
}

int SerialRequest::request() const {
    return request_;
}

//----------------------------------------------------------------------------------------------------------------------

SendRequest::SendRequest(const void* buf, size_t count, Data::Code type, int tag) :
    buf_(0),
    count_(count),
    tag_(tag),
    type_(type) {
    buf_ = ::operator new( count * dataSize[type] );
    memcpy( buf_, buf, count * dataSize[type] );
}

SendRequest::~SendRequest() {
    ::operator delete(buf_);
}

//----------------------------------------------------------------------------------------------------------------------

ReceiveRequest::ReceiveRequest(void* buf, size_t count, Data::Code type, int tag) :
    buf_(buf),
    count_(count),
    tag_(tag),
    type_(type) {
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
