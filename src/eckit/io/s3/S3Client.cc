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

#include "eckit/io/s3/S3Client.h"

#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"

namespace eckit {

namespace {

inline auto makeSharedS3Client(const S3Types type) -> std::shared_ptr<S3Client> {
    if (type == S3Types::AWS) { return std::make_shared<S3ClientAWS>(); }
    throw S3SeriousBug("Unkown S3 client type!", Here());
}

inline auto makeUniqueS3Client(const S3Types type) -> std::unique_ptr<S3Client> {
    if (type == S3Types::AWS) { return std::make_unique<S3ClientAWS>(); }
    throw S3SeriousBug("Unkown S3 client type!", Here());
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

S3Client::S3Client(std::shared_ptr<S3Context> context): context_(context) { }

S3Client::~S3Client() = default;

//----------------------------------------------------------------------------------------------------------------------

void S3Client::print(std::ostream& out) const {
    out << "S3Client[type=";
    const auto type = context_->getType();
    if (type == S3Types::AWS) {
        out << "AWS";
    } else if (type == S3Types::REST) {
        out << "REST";
    } else {
        out << "REST";
    }
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

auto S3Client::makeShared(const S3Config& config) -> std::shared_ptr<S3Client> {
    auto client = makeSharedS3Client(config.type);
    client->configure(config);
    return client;
}

//----------------------------------------------------------------------------------------------------------------------

auto S3Client::makeUnique(const S3Config& config) -> std::unique_ptr<S3Client> {
    auto client = makeUniqueS3Client(config.type);
    client->configure(config);
    return client;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
