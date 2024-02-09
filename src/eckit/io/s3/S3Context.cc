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

#include "eckit/io/s3/S3Context.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/s3/aws/S3ContextAWS.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Context::S3Context(const S3Types type): type_(type) { }

S3Context::~S3Context() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Context::makeShared(S3Types type) -> std::shared_ptr<S3Context> {
    // AWS SDK API
    if (type == S3Types::AWS) { return S3ContextAWS::makeShared(); }

    // REST API
    if (type == S3Types::REST) { NOTIMP; }

    return {};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
