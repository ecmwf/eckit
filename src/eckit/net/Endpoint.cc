/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/net/Endpoint.h"

#include <ostream>

#include "eckit/serialisation/Stream.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Endpoint::Endpoint(const std::string& host, int port) :
    hostname_(host),
    port_(port) {}

Endpoint::Endpoint(Stream& s) {
    s >> hostname_;
    s >> port_;
}

Endpoint::Endpoint() : port_(0) {}

bool Endpoint::operator==(const Endpoint& other) {
    return (port_ == other.port_ && hostname_ == other.hostname_);
}

void Endpoint::print(std::ostream& os) const {
    os << hostname_ << ":" << port_;
}

void Endpoint::encode(Stream &s) const {
    s << hostname_;
    s << port_;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
