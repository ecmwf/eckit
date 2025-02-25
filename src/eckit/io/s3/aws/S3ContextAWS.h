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

#include <aws/core/Aws.h>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Provides singleton for AWS API init/shutdown
class S3ContextAWS {
public:  // methods
    S3ContextAWS(const S3ContextAWS&)            = delete;
    S3ContextAWS& operator=(const S3ContextAWS&) = delete;
    S3ContextAWS(S3ContextAWS&&)                 = delete;
    S3ContextAWS& operator=(S3ContextAWS&&)      = delete;

    static S3ContextAWS& instance();

private:  // methods
    explicit S3ContextAWS();

    ~S3ContextAWS();

private:  // members
    Aws::SDKOptions options_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
