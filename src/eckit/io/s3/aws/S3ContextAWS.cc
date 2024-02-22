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

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3ContextAWS& S3ContextAWS::instance() {
    static S3ContextAWS ctx;
    return ctx;
}

//----------------------------------------------------------------------------------------------------------------------

S3ContextAWS::S3ContextAWS() {
    Aws::InitAPI(options_);
}

S3ContextAWS::~S3ContextAWS() {
    Aws::ShutdownAPI(options_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
