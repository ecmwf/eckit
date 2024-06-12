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

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamSession.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace eckit {

class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class FamName {
public:  // methods
    FamName(FamSession::SPtr session, const std::string& path);

    FamName(const net::Endpoint& endpoint, const std::string& path);

    explicit FamName(const URI& uri);

    explicit FamName(const std::string& uri);

    virtual ~FamName();

    virtual auto asString() const -> std::string;

    auto uri() const -> URI;

    auto with(std::string_view regionName) -> FamName&;

    auto with(std::string_view regionName, std::string_view objectName) -> FamName&;

    // region

    auto lookupRegion() const -> FamRegion;

    auto createRegion(fam::size_t regionSize, fam::perm_t regionPerms, bool overwrite = false) -> FamRegion;

    auto existsRegion() const -> bool;

    auto nameRegion() const -> const std::string& { return regionName_; }

    // object

    auto lookupObject() const -> FamObject;

    auto allocateObject(fam::size_t objectSize, const bool overwrite = false) -> FamObject;

    auto existsObject() const -> bool;

    auto nameObject() const -> const std::string& { return objectName_; }

    // datahandle

    [[nodiscard]]
    auto dataHandle(bool overwrite = false) const -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const Offset& offset, const Length& length) const -> DataHandle*;

protected:  // methods
    auto config() const -> const FamConfig&;

    virtual void print(std::ostream& out) const;

private:  // methods
    void parsePath(const std::string& path);

    friend std::ostream& operator<<(std::ostream& out, const FamName& name);

private:  // members
    FamSession::SPtr session_;

    std::string regionName_;
    std::string objectName_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
