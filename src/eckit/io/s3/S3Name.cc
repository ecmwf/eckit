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

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Handle.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Name::S3Name(const URI& uri): client_(S3Client::makeUnique({uri})) {
    parse(uri.name());
}

S3Name::~S3Name() = default;

//----------------------------------------------------------------------------------------------------------------------

void S3Name::parse(const std::string& path) {
    const auto pairs = Tokenizer("/").tokenize(path);

    ASSERT(pairs.size() == 2);

    bucket_ = pairs[0];
    object_ = pairs[1];
}

void S3Name::print(std::ostream& out) const {
    out << "S3Name[bucket=" << bucket_ << ",object=" << object_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

void S3Name::put(const void* buffer, const uint64_t length) const {
    client_->putObject(bucket_, object_, buffer, length);
}

auto S3Name::bucketExists() const -> bool {
    return client_->bucketExists(bucket_);
}

auto S3Name::exists() const -> bool {
    return client_->objectExists(bucket_, object_);
}

auto S3Name::size() const -> Length {
    return client_->objectSize(bucket_, object_);
}

auto S3Name::dataHandle() -> DataHandle* {
    return new S3Handle(*this);
}

auto S3Name::asString() const -> std::string {
    return bucket_ + "/" + object_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
