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

#include <string>

#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamProperty.h"

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
