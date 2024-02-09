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

/// @file   S3ContextAWS.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Context.h"

#include <aws/core/Aws.h>

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3ContextAWS: public S3Context {
public:  // methods
    NO_MOVE(S3ContextAWS)

    explicit S3ContextAWS(const Aws::SDKOptions& options);

    ~S3ContextAWS();

private:  // methods
    friend S3Context;

    static auto makeShared() -> std::shared_ptr<S3Context>;

private:  // members
    Aws::SDKOptions options_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
