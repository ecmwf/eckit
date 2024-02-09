/*
 *  (C) Copyright 1996- ECMWF.
 *
 *  This software is licensed under the terms of the Apache Licence Version 2.0
 *  which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *  In applying this licence, ECMWF does not waive the privileges and immunities
 *  granted to it by virtue of its status as an intergovernmental organisation nor
 *  does it submit to any jurisdiction.
 */

/// @file   S3Name.h
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Macros.h"

#include <string>

namespace eckit {

class URI;
class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // methods
    explicit S3Name(const URI& uri);

    S3Name(const S3Config& config, const std::string& bucketName, const std::string& objectName);

    ~S3Name();

    auto getConfig() const -> const S3Config& { return config_; }

    auto exists() -> bool;

    NODISCARD
    auto dataHandle() const -> DataHandle*;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

private:  // methods
    void print(std::ostream& out) const;

private:  // members
    S3Config    config_;
    std::string bucket_;
    std::string object_;

    // std::unique_ptr<S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
