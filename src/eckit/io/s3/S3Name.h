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

#include "eckit/net/Endpoint.h"

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace eckit {

class S3Client;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // types
    static constexpr auto type = "s3";

public:  // methods
    explicit S3Name(const URI& uri);

    S3Name(const net::Endpoint& endpoint, std::string name);

    // rules

    S3Name(const S3Name&)            = default;
    S3Name& operator=(const S3Name&) = default;

    S3Name(S3Name&&)            = delete;
    S3Name& operator=(S3Name&&) = delete;

    virtual ~S3Name();

    // accessors

    auto uri() const -> URI;

    auto endpoint() const -> const net::Endpoint& { return endpoint_; }

    virtual auto exists() const -> bool = 0;

    virtual auto asString() const -> std::string;

protected:  // methods
    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name);

    [[nodiscard]]
    auto parseName() const -> std::vector<std::string>;

    auto client() const -> S3Client&;

private:  // members
    net::Endpoint endpoint_;
    std::string   name_;

    mutable std::shared_ptr<S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
