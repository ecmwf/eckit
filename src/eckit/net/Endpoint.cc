// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/Endpoint.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

Endpoint::Endpoint(const URI& uri) : host_(uri.host()), port_(uri.port()) {
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
