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
#include "eckit/io/s3/S3ObjectPath.h"

#include <ostream>
#include <string>

namespace eckit {

class Offset;
class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class S3ObjectName : public S3Name {
public:  // helpers
    static auto parse(const std::string& name) -> S3ObjectPath;

public:  // methods
    S3ObjectName(const net::Endpoint& endpoint, S3ObjectPath path);

    explicit S3ObjectName(const URI& uri);

    auto uri() const -> URI override;

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

    auto path() const -> const S3ObjectPath& { return path_; }

    auto name() const -> const std::string& { return path_.object; }

    auto bucket() const -> const std::string& { return path_.bucket; }

    auto bucketExists() const -> bool;

private:  // methods
    void print(std::ostream& out) const override;

private:  // members
    S3ObjectPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
