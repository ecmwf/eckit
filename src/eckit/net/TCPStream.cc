/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/net/TCPStream.h"

namespace eckit {
namespace net {


TCPStream::TCPStream(net::TCPSocket& socket) :
    socket_(socket) {}

TCPStream::~TCPStream() {}

void TCPStream::closeOutput() {
    socket_.closeOutput();
}
//----------------------------------------------------------------------------------------------------------------------
// Tricky solution to be removed when 'mutable' is available
//
std::string TCPStreamBase::nonConstName() {
    std::ostringstream r;
    r << "TCPStream[" << socket() << "]";
    return r.str();
}

std::string TCPStreamBase::name() const {
    return ((TCPStreamBase*)this)->nonConstName();
}

SharedTCPStream::SharedTCPStream(net::TCPSocket& s) :
    TCPStream(s) {}

SharedTCPStream::~SharedTCPStream() {}


}  // namespace net
}  // namespace eckit
