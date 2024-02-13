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

/// @file   S3Name.h
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#pragma once

#include "eckit/io/Length.h"

#include <memory>
#include <string>

namespace eckit {

class URI;
class DataHandle;
class S3Client;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // methods
    explicit S3Name(const URI& uri);

    auto put(const void* buffer, long length) const -> long;

    auto get(void* buffer, long length) const -> long;

    auto get(void* buffer, const long offset, const long length) const -> long;

    auto bucket() const -> const std::string& { return bucket_; }

    auto object() const -> const std::string& { return object_; }

    auto bucketExists() const -> bool;

    auto exists() const -> bool;

    auto size() const -> Length;

    [[nodiscard]]
    auto dataHandle() -> DataHandle*;

    auto asString() const -> std::string;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

private:  // methods
    void parse(const std::string& path);

    void print(std::ostream& out) const;

private:  // members
    std::string bucket_;
    std::string object_;

    std::shared_ptr<S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
