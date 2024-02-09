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

//----------------------------------------------------------------------------------------------------------------------

S3Client::S3Client(std::shared_ptr<S3Context> context): context_(context) { }

S3Client::~S3Client() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Client::makeUnique(const S3Types type) -> std::unique_ptr<S3Client> {
    if (type == S3Types::AWS) { return std::make_unique<S3ClientAWS>(); }
    throw S3SeriousBug("Unkown S3 client type!", Here());
}

auto S3Client::makeUnique(const S3Config& config) -> std::unique_ptr<S3Client> {
    auto client = makeUnique(config.type());
    client->configure(config);
    return client;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
