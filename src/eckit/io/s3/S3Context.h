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

/// @file   S3Context.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Macros.h"

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Context {
public:  // methods
    NO_COPY_NO_MOVE(S3Context)

    explicit S3Context(S3Types type);

    virtual ~S3Context();

    NODISCARD
    auto getType() const -> S3Types { return type_; }

private:  // methods
    friend class S3Session;

    static auto makeShared(S3Types type) -> std::shared_ptr<S3Context>;

private:  // members
    const S3Types type_ {S3Types::NONE};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
