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

/// @file   S3BucketName.h
/// @author Metin Cakircali
/// @author Nicolau Manubens
/// @date   Feb 2024

#pragma once

#include "eckit/io/s3/S3BucketPath.h"
#include "eckit/io/s3/S3Name.h"
#include "eckit/net/Endpoint.h"

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace eckit {

class S3ObjectName;

//----------------------------------------------------------------------------------------------------------------------

class S3BucketName : public S3Name {
public:  // factory
    static auto parse(const std::string& name) -> S3BucketPath;

    auto makeObject(const std::string& object) const -> std::unique_ptr<S3ObjectName>;

public:  // methods
    S3BucketName(const net::Endpoint& endpoint, S3BucketPath path);

    explicit S3BucketName(const URI& uri);

    // accessors

    auto asString() const -> std::string override;

    auto uri() const -> URI override;

    auto exists() const -> bool override;

    auto path() const -> const S3BucketPath& { return path_; }

    auto bucket() const -> const std::string& { return path_.bucket; }

    /// @todo return S3 object iterator but first add prefix
    auto listObjects() const -> std::vector<std::string>;

    // modifiers

    void create();

    void destroy();

    void ensureCreated();

    void ensureDestroyed();

private:  // methods
    void print(std::ostream& out) const override;

private:  // members
    S3BucketPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
