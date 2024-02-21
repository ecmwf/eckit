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

#include <string>

namespace eckit {

class S3Client;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // methods
    explicit S3Name(const URI& uri);

    S3Name(const net::Endpoint& endpoint, const std::string& name);

    virtual ~S3Name();

    auto uri() const -> URI;

    auto endpoint() const -> const net::Endpoint& { return endpoint_; }

    virtual auto exists() const -> bool = 0;

    virtual auto asString() const -> std::string;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

protected:  // methods
    virtual void print(std::ostream& out) const;

    [[nodiscard]]
    auto parseName() const -> std::vector<std::string>;

    auto client() const -> std::shared_ptr<S3Client>;

private:  // members
    const net::Endpoint endpoint_;
    const std::string   name_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
