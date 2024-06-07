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

#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamSession.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamName {
public:  // methods
    explicit FamName(const net::Endpoint& endpoint, std::string name);

    explicit FamName(const URI& uri);

    virtual ~FamName();

    auto uri() const -> URI;

    virtual auto asString() const -> std::string;

    virtual auto exists() -> bool = 0;

    virtual auto lookup() -> bool = 0;

    virtual void create(const FamProperty& property) = 0;

    virtual void destroy() = 0;

protected:  // methods
    [[nodiscard]]
    auto parseName() const -> std::vector<std::string>;

    auto config() const -> const FamConfig&;

    auto session() const -> Session;

    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamName& name);

private:  // members
    const FamConfig   config_;
    const std::string name_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
