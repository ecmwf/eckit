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
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

#include "eckit/io/s3/S3Name.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3BucketName.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3ObjectName.h"
#include "eckit/io/s3/S3ObjectPath.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/utils/Tokenizer.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto S3Name::parse(const std::string& name) -> std::vector<std::string> {
    return Tokenizer("/").tokenize(name);
}

auto S3Name::make(const net::Endpoint& endpoint, const std::string& name) -> std::unique_ptr<S3Name> {
    const auto names = parse(name);
    switch (names.size()) {
        case 1:  return std::make_unique<S3BucketName>(endpoint, names[0]);
        case 2:  return std::make_unique<S3ObjectName>(endpoint, S3ObjectPath {names[0], names[1]});
        default: throw SeriousBug("Could not parse S3 name: " + name, Here());
    }
}

//----------------------------------------------------------------------------------------------------------------------

S3Name::S3Name(const net::Endpoint& endpoint) : endpoint_ {endpoint} { }

S3Name::S3Name(const URI& uri) : endpoint_ {uri.host(), uri.port()} {
    /// @todo is "s3://endpoint/bucket/object" a valid URI ?
    ASSERT(uri.scheme() == type);
}

S3Name::~S3Name() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Name::uri() const -> URI {
    return {type, endpoint_.host(), endpoint_.port()};
}

auto S3Name::asString() const -> std::string {
    return std::string(endpoint_);
}

void S3Name::print(std::ostream& out) const {
    out << ",endpoint=" << endpoint_ << ",]";
}

//----------------------------------------------------------------------------------------------------------------------

auto S3Name::client() const -> S3Client& {
    return *S3Session::instance().getClient(endpoint_);
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const S3Name& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
