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

#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamSession.h"

#include <iosfwd>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamName {
public:  // methods
    FamName(const net::Endpoint& endpoint, FamPath path);

    FamName(const URI& uri);

    virtual ~FamName();

    virtual auto exists() const -> bool = 0;

    auto asString() const -> std::string;

    auto uri() const -> URI;

    auto uriBelongs(const URI& uri) const -> bool;

    auto endpoint() const -> const net::Endpoint& { return endpoint_; }

    auto path() const -> const FamPath& { return path_; }

protected:  // methods
    auto session() const -> FamSession::SPtr;

    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamName& name);

protected:  // members
    net::Endpoint endpoint_;

    FamPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
