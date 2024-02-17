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

#include "eckit/io/s3/S3BucketName.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3BucketName::S3BucketName(const URI& uri): S3Name(uri) {
    parse();
}

//----------------------------------------------------------------------------------------------------------------------

auto S3BucketName::asString() const -> std::string {
    return S3Name::asString() + "/" + bucket_;
}

void S3BucketName::print(std::ostream& out) const {
    out << "S3BucketName[bucket=" << bucket_;
    S3Name::print(out);
}

void S3BucketName::parse() {
    const auto pairs = parseName();
    if (pairs.empty()) { throw S3SeriousBug("Could not parse bucket name!", Here()); }
    bucket_ = pairs[0];
}

//----------------------------------------------------------------------------------------------------------------------

auto S3BucketName::exists() const -> bool {
    return client()->bucketExists(bucket_);
}

void S3BucketName::create() {
    client()->createBucket(bucket_);
}

void S3BucketName::destroy() {
    client()->deleteBucket(bucket_);
}

void S3BucketName::ensureCreated() {
    try {
        create();
    }
    catch (S3EntityAlreadyExists& e) {
        LOG_DEBUG_LIB(LibEcKit) << e.what() << "\n";
    }
}

void S3BucketName::ensureDestroyed() {
    try {
        client()->emptyBucket(bucket_);
        client()->deleteBucket(bucket_);
    }
    catch (S3EntityNotFound& e) {
        LOG_DEBUG_LIB(LibEcKit) << e.what() << "\n";
    }
}

auto S3BucketName::listObjects() const -> std::vector<std::string> {
    return client()->listObjects(bucket_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
