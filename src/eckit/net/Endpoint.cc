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

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

#include <ostream>

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

Endpoint::Endpoint(const URI& uri): host_(uri.host()), port_(uri.port()) {
    validate();
}

Endpoint::Endpoint(const std::string& s) {
    Tokenizer tokenize(":");
    std::vector<std::string> tokens;
    tokenize(s, tokens);
    ASSERT(tokens.size() == 2);
    host_ = tokens[0];
    port_ = Translator<std::string, int>()(tokens[1]);
    validate();
}

Endpoint::Endpoint(const std::string& host, int port) : host_(host), port_(port) {
    validate();
}

Endpoint::Endpoint(Stream& s) {
    s >> host_;
    s >> port_;
    validate();
}

Endpoint::Endpoint() : port_(0) {
    validate();
}

bool Endpoint::operator==(const net::Endpoint& other) const {
    return (port_ == other.port_ && hostname() == other.hostname());
}

void Endpoint::print(std::ostream& os) const {
    os << hostname() << ":" << port_;
}

void Endpoint::encode(Stream& s) const {
    s << host_;
    s << port_;
}

void Endpoint::validate() const {
    // IP ranges are valid 1 - 65535
    if (port_ < 0 or port_ > 65535) {
        std::ostringstream msg;
        msg << "Invalid port number " << port_;
        throw eckit::BadValue(msg.str(), Here());
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
