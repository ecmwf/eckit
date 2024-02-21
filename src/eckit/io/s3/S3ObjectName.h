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

/// @file   S3ObjectName.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Name.h"

namespace eckit {

class Offset;
class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class S3ObjectName: public S3Name {
public:  // methods
    explicit S3ObjectName(const URI& uri);

    S3ObjectName(const net::Endpoint& endpoint, const std::string& bucket, const std::string& object);

    auto size() const -> long long;

    auto exists() const -> bool override;

    void remove();

    auto put(const void* buffer, long length) const -> long long;

    auto get(void* buffer, long offset, long length) const -> long long;

    [[nodiscard]]
    auto dataHandle() -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const Offset& offset) -> DataHandle*;

    auto asString() const -> std::string override;

    auto name() const -> const std::string& { return object_; }

    auto bucket() const -> const std::string& { return bucket_; }

    auto bucketExists() const -> bool;

private:  // methods
    void print(std::ostream& out) const override;

    void parse();

private:  // members
    std::string bucket_;
    std::string object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
