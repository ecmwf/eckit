// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamRegionName.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include <string>

#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamObjectName.h"

namespace eckit {

class URI;
class FamRegion;

//----------------------------------------------------------------------------------------------------------------------

class FamRegionName : public FamName {
public:  // methods

    using FamName::FamName;

    FamRegionName& withRegion(const std::string& region_name);

    FamObjectName object(const std::string& object_name) const;

    FamRegion lookup() const;

    FamRegion create(fam::size_t region_size, fam::perm_t region_perm, bool overwrite = false) const;

    bool exists() const override;

    bool uriBelongs(const URI& uri) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
