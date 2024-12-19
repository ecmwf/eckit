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
public:  // statics
    static constexpr auto type = "s3";

    static auto parse(const std::string& name) -> std::vector<std::string>;

    static auto make(const net::Endpoint& endpoint, const std::string& name) -> std::unique_ptr<S3Name>;

public:  // methods
    explicit S3Name(const net::Endpoint& endpoint);

    explicit S3Name(const URI& uri);

    // rules

    S3Name(const S3Name&)            = default;
    S3Name& operator=(const S3Name&) = default;

    S3Name(S3Name&&)            = delete;
    S3Name& operator=(S3Name&&) = delete;

    virtual ~S3Name();

    // accessors

    void endpoint(const net::Endpoint& endpoint) { endpoint_ = endpoint; }

    auto endpoint() const -> const net::Endpoint& { return endpoint_; }

    virtual auto uri() const -> URI;

    virtual auto exists() const -> bool = 0;

    virtual auto asString() const -> std::string;

protected:  // methods
    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name);

    auto client() const -> S3Client&;

private:  // members
    net::Endpoint endpoint_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
