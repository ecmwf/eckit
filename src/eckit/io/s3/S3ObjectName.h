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

#include <iosfwd>
#include <string>

namespace eckit {

class Offset;
class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class S3ObjectName : public S3Name {
public:  // factory
    static auto parse(const std::string& name) -> S3ObjectPath;

public:  // methods
    S3ObjectName(const net::Endpoint& endpoint, S3ObjectPath path);

    explicit S3ObjectName(const URI& uri);

    // accessors

    auto uri() const -> URI override;

    auto exists() const -> bool override;

    auto asString() const -> std::string override;

    auto size() const -> long long;

    auto path() const -> const S3ObjectPath& { return path_; }

    auto bucketExists() const -> bool;

    // modifiers

    void remove();

    // I/O

    auto put(const void* buffer, long length) const -> long long;

    auto get(void* buffer, long offset, long length) const -> long long;

    [[nodiscard]]
    auto dataHandle() const -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const Offset& offset) const -> DataHandle*;

private:  // methods
    void print(std::ostream& out) const override;

private:  // members
    S3ObjectPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
