// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamName.h"

#include <ostream>
#include <sstream>
#include <string>
#include <utility>

#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamSessionManager.h"
#include "eckit/io/fam/FamTypes.h"
#include "eckit/net/Endpoint.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(const net::Endpoint& endpoint, FamPath path) : endpoint_{endpoint}, path_{std::move(path)} {}

FamName::FamName(const URI& uri) : FamName(uri.endpoint(), FamPath(uri)) {}

FamName::FamName(Stream& stream) : endpoint_{stream}, path_{stream} {}

FamName::~FamName() = default;

//----------------------------------------------------------------------------------------------------------------------

FamSessionManager::Session FamName::session() const {
    return FamSessionManager::instance().session(endpoint_);
}

std::string FamName::asString() const {
    std::ostringstream oss;
    oss << fam::scheme << "://" << endpoint_ << path_;
    return oss.str();
}

URI FamName::uri() const {
    return {fam::scheme, endpoint_, path_.asString()};
}

void FamName::print(std::ostream& out) const {
    out << "endpoint=" << endpoint_ << ", path=" << path_;
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

Stream& operator<<(Stream& out, const FamName& name) {
    out << name.endpoint_;
    out << name.path_;
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
