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

#include "eckit/io/s3/S3ObjectName.h"

#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Handle.h"
#include "eckit/io/s3/S3Name.h"
#include "eckit/io/s3/S3ObjectPath.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/net/Endpoint.h"

#include <ostream>
#include <string>
#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto S3ObjectName::parse(const std::string& name) -> S3ObjectPath {
    const auto parsed = S3Name::parse(name);
    if (parsed.size() != 2) { throw S3SeriousBug("Could not parse bucket/object from name: " + name, Here()); }
    return {parsed[0], parsed[1]};
}

//----------------------------------------------------------------------------------------------------------------------

S3ObjectName::S3ObjectName(const net::Endpoint& endpoint, S3ObjectPath path)
    : S3Name(endpoint), path_ {std::move(path)} { }

S3ObjectName::S3ObjectName(const URI& uri) : S3Name(uri), path_ {parse(uri.name())} { }

//----------------------------------------------------------------------------------------------------------------------

auto S3ObjectName::uri() const -> URI {
    auto uri = S3Name::uri();
    uri.path(path_);
    return uri;
}

auto S3ObjectName::asString() const -> std::string {
    return S3Name::asString() + path_.asString();
}

void S3ObjectName::print(std::ostream& out) const {
    out << "S3ObjectName[";
    S3Name::print(out);
    out << ',' << path_ << ']';
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ObjectName::size() const -> long long {
    return client().objectSize(path_);
}

auto S3ObjectName::exists() const -> bool {
    return client().objectExists(path_);
}

auto S3ObjectName::bucketExists() const -> bool {
    return client().bucketExists(path_.bucket);
}

void S3ObjectName::remove() {
    client().deleteObject(path_);
}

auto S3ObjectName::put(const void* buffer, const long length) const -> long long {
    return client().putObject(path_, buffer, length);
}

auto S3ObjectName::get(void* buffer, const long offset, const long length) const -> long long {
    return client().getObject(path_, buffer, offset, length);
}

auto S3ObjectName::dataHandle() const -> DataHandle* {
    return new S3Handle(*this);
}

auto S3ObjectName::dataHandle(const Offset& offset) const -> DataHandle* {
    return new S3Handle(*this, offset);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
