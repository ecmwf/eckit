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
#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamSession.h"

#include <iosfwd>
#include <string>

namespace eckit {

class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class FamName {
public:  // methods
    FamName(const net::Endpoint& endpoint, FamPath path);

    FamName(const URI& uri);

    auto asString() const -> std::string;

    auto uri() const -> URI;

    auto withEndpoint(const net::Endpoint& endpoint) -> FamName&;

    auto withRegion(std::string_view regionName) -> FamName&;

    auto withObject(std::string_view objectName) -> FamName&;

    auto path() const -> const FamPath& { return path_; }

    // region

    auto lookupRegion() const -> FamRegion;

    auto createRegion(fam::size_t regionSize, fam::perm_t regionPerm, bool overwrite = false) const -> FamRegion;

    auto existsRegion() const -> bool;

    // object

    auto lookupObject() const -> FamObject;

    auto allocateObject(fam::size_t objectSize, const bool overwrite = false) const -> FamObject;

    auto existsObject() const -> bool;

    // datahandle

    [[nodiscard]]
    auto dataHandle(bool overwrite = false) const -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const Offset& offset, const Length& length) const -> DataHandle*;

protected:  // methods
    auto session() const -> FamSession::SPtr;

    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamName& name);

private:  // members
    net::Endpoint endpoint_;

    FamPath path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
