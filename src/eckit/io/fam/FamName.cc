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

#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"

#include <ostream>
#include <sstream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(const net::Endpoint& endpoint, FamPath path): endpoint_ {endpoint}, path_ {std::move(path)} { }

FamName::FamName(const URI& uri): FamName(uri.endpoint(), uri) { }

FamName::~FamName() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamName::session() const -> FamSession::SPtr {
    return FamSession::instance().getOrAdd({endpoint_});
}

auto FamName::asString() const -> std::string {
    std::ostringstream oss;
    oss << FamPath::SCHEME << "://" << endpoint_ << path_;
    return oss.str();
}

auto FamName::uri() const -> URI {
    return {FamPath::SCHEME, endpoint_, path_};
}

auto FamName::uriBelongs(const URI& uri) const -> bool {
    return (FamPath(uri).regionName == path_.regionName);
}

void FamName::print(std::ostream& out) const {
    out << "endpoint=" << endpoint_ << ", path=" << path_;
}

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
