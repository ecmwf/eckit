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

#include <string>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamRegionName& FamRegionName::withRegion(const std::string& region_name) {
    path().regionName = region_name;
    return *this;
}

FamObjectName FamRegionName::object(const std::string& object_name) const {
    return {endpoint(), {path().regionName, object_name}};
}

FamRegion FamRegionName::lookup() const {
    return session()->lookupRegion(path().regionName);
}

FamRegion FamRegionName::create(const fam::size_t region_size, const fam::perm_t region_perm,
                                const bool overwrite) const {
    if (overwrite) {
        return session()->ensureCreateRegion(region_size, region_perm, path().regionName);
    }
    return session()->createRegion(region_size, region_perm, path().regionName);
}

bool FamRegionName::exists() const {
    try {
        return lookup().exists();
    }
    catch (const NotFound& not_found) {
        Log::debug<LibEcKit>() << not_found << '\n';
    }
    catch (const PermissionDenied& permission_denied) {
        Log::debug<LibEcKit>() << permission_denied << '\n';
    }
    return false;
}

bool FamRegionName::uriBelongs(const URI& uri) const {
    return uri.scheme() == FamPath::scheme && uri.endpoint() == endpoint() &&
           FamPath(uri).regionName == path().regionName;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
