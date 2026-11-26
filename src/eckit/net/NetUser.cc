// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/net/NetUser.h"
#include "eckit/io/SockBuf.h"
#include "eckit/net/TCPStream.h"

namespace eckit::net {

NetUser::NetUser(net::TCPSocket& protocol) : protocol_(protocol) {
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

void NetUser::reject(const std::string& reason) {
    SockBuf buf(protocol_);
    std::ostream out(&buf);
    std::istream in(&buf);
    InstantTCPStream stream(protocol_);

    refuse(stream, in, out, reason);
}

void NetUser::refuse(Stream&, std::istream&, std::ostream&, const std::string& reason) {
    Log::warning() << "Refusing connection from " << protocol_.remoteHost() << ": " << reason << std::endl;
}

}  // namespace eckit::net
