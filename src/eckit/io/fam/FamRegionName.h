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

/// @file   FamRegionName.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamRegion.h"

#include <iosfwd>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamRegionName: public FamName {
public:  // methods
    using FamName::FamName;

    ~FamRegionName() = default;

    auto withRegion(const std::string& regionName) -> FamRegionName&;

    auto object(const std::string& objectName) const -> FamObjectName;

    auto lookup() const -> FamRegion;

    auto create(fam::size_t regionSize, fam::perm_t regionPerm, bool overwrite = false) const -> FamRegion;

    auto exists() const -> bool override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
