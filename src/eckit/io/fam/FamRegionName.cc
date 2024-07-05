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

#include "eckit/io/fam/FamRegionName.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/log/Log.h"

#include <iostream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto FamRegionName::withRegion(std::string_view regionName) -> FamRegionName& {
    path().regionName = regionName;
    return *this;
}

auto FamRegionName::lookup() const -> FamRegion {
    return session()->lookupRegion(path().regionName);
}

auto FamRegionName::create(const fam::size_t regionSize,
                           const fam::perm_t regionPerm,
                           const bool        overwrite) const -> FamRegion {
    if (overwrite) { return session()->ensureCreateRegion(regionSize, regionPerm, path().regionName); }
    return session()->createRegion(regionSize, regionPerm, path().regionName);
}

auto FamRegionName::exists() const -> bool {
    try {
        return lookup().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM region [" << path().regionName << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM region [" << path().regionName << "] is not accessible!\n";
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit