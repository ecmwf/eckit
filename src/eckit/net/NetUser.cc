/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/NetUser.h"
#include "eckit/io/SockBuf.h"
#include "eckit/net/TCPStream.h"

namespace eckit {
namespace net {

NetUser::NetUser(net::TCPSocket& protocol) :
    protocol_(protocol) {
    Log::status() << "New connection from " << protocol_.remoteHost() << std::endl;
}

NetUser::~NetUser() {
    Log::status() << "End connection from " << protocol_.remoteHost() << std::endl;
}

void NetUser::run() {
    SockBuf buf(protocol_);
    std::ostream out(&buf);
    std::istream in(&buf);
    InstantTCPStream stream(protocol_);

    serve(stream, in, out);
}


}  // namespace net
}  // namespace eckit
