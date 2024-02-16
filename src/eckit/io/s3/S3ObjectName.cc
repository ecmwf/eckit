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

#include "eckit/config/LibEcKit.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Handle.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3ObjectName::S3ObjectName(const URI& uri): S3Name(uri) {
    parse();
}

//----------------------------------------------------------------------------------------------------------------------

void S3ObjectName::print(std::ostream& out) const {
    out << "S3ObjectName[bucket=" << bucket_;
    S3Name::print(out);
    out << "]";
}

void S3ObjectName::parse() {
    const auto pairs = parseName();
    if (pairs.size() != 2) { throw S3SeriousBug("Could not parse bucket and object names!", Here()); }
    bucket_ = pairs[0];
    object_ = pairs[1];
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ObjectName::uri() const -> URI {
    return asString();
}

auto S3ObjectName::size() const -> long long {
    return client()->objectSize(bucket_, object_);
}

auto S3ObjectName::exists() const -> bool {
    return client()->objectExists(bucket_, object_);
}

void S3ObjectName::remove() {
    client()->deleteObject(bucket_, object_);
}

auto S3ObjectName::put(const void* buffer, const long length) const -> long long {
    return client()->putObject(bucket_, object_, buffer, length);
}

auto S3ObjectName::get(void* buffer, const long offset, const long length) const -> long long {
    return client()->getObject(bucket_, object_, buffer, offset, length);
}

auto S3ObjectName::dataHandle() -> DataHandle* {
    return new S3Handle(*this);
}

auto S3ObjectName::dataHandle(const Offset& offset) -> DataHandle* {
    return new S3Handle(*this, offset);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
