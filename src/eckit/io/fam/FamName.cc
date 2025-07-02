/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/net/Endpoint.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(const net::Endpoint& endpoint, FamPath path) : endpoint_{endpoint}, path_{std::move(path)} {}

FamName::FamName(const URI& uri) : FamName(uri.endpoint(), uri) {}

FamName::FamName(Stream& stream) : endpoint_{stream}, path_{stream} {}

FamName::~FamName() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamName::session() const -> FamSession::SharedPtr {
    return FamSession::instance().getOrAdd({endpoint_});
}

auto FamName::asString() const -> std::string {
    std::ostringstream oss;
    oss << FamPath::scheme << "://" << endpoint_ << path_;
    return oss.str();
}

auto FamName::uri() const -> URI {
    return {FamPath::scheme, endpoint_, path_.asString()};
}

void FamName::print(std::ostream& out) const {
    out << "endpoint=" << endpoint_ << ", path=" << path_;
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

auto operator<<(Stream& out, const FamName& name) -> Stream& {
    out << name.endpoint_;
    out << name.path_;
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
