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
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamName.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include <iosfwd>
#include <string>

#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamSessionManager.h"
#include "eckit/net/Endpoint.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamName {
public:  // methods

    FamName(const net::Endpoint& endpoint, FamPath path);
    FamName(const net::Endpoint& endpoint, const std::string& path) : FamName(endpoint, FamPath(path)) {}

    explicit FamName(const URI& uri);

    explicit FamName(Stream& stream);

    // rules
    FamName(const FamName&)            = default;
    FamName& operator=(const FamName&) = default;
    FamName(FamName&&)                 = default;
    FamName& operator=(FamName&&)      = default;

    virtual ~FamName();

    virtual bool exists() const = 0;

    /// @todo implement
    // virtual FamItem lookup() const = 0;

    std::string asString() const;

    URI uri() const;

    const net::Endpoint& endpoint() const { return endpoint_; }

    const FamPath& path() const { return path_; }

protected:  // methods

    FamSessionManager::Session session() const;

    FamPath& path() { return path_; }

    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamName& name);

    friend Stream& operator<<(Stream& out, const FamName& name);

private:  // members

    net::Endpoint endpoint_;

    FamPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
