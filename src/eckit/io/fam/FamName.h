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

    FamName(const URI& uri);

    FamName(Stream& stream);

    virtual ~FamName();

    virtual auto exists() const -> bool = 0;

    /// @todo implement
    // virtual auto lookup() const -> FamItem = 0;

    auto asString() const -> std::string;

    auto uri() const -> URI;

    auto endpoint() const -> const net::Endpoint& { return endpoint_; }

    auto path() const -> const FamPath& { return path_; }

protected:  // methods

    auto session() const -> FamSessionManager::FamSession;

    auto path() -> FamPath& { return path_; }

    virtual void print(std::ostream& out) const;

    friend auto operator<<(std::ostream& out, const FamName& name) -> std::ostream&;

    friend auto operator<<(Stream& out, const FamName& name) -> Stream&;

private:  // members

    net::Endpoint endpoint_;

    FamPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
