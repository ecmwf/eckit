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

#include "eckit/io/s3/aws/S3ContextAWS.h"

#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3ContextAWS::S3ContextAWS(const Aws::SDKOptions& options): S3Context(S3Types::AWS), options_(options) {
    Aws::InitAPI(options_);
}

S3ContextAWS::~S3ContextAWS() {
    Aws::ShutdownAPI(options_);
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ContextAWS::makeShared() -> std::shared_ptr<S3Context> {
    Aws::SDKOptions options;
    /// @todo remove debug stuff below
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    return std::make_shared<S3ContextAWS>(options);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
