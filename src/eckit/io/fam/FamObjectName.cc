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

#include "eckit/io/fam/FamObjectName.h"

#include <ostream>
#include <string>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamHandle.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamObjectName& FamObjectName::withObject(const std::string& object_name) {
    path().objectName = object_name;
    return *this;
}

FamObjectName& FamObjectName::withUUID() {
    return withObject(path().generateUUID());
}

FamObject FamObjectName::lookup() const {
    return session()->lookupObject(path().regionName, path().objectName);
}

FamObject FamObjectName::allocate(const fam::size_t object_size, const bool overwrite) const {
    return session()->lookupRegion(path().regionName).allocateObject(object_size, path().objectName, overwrite);
}

bool FamObjectName::exists() const {
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

//----------------------------------------------------------------------------------------------------------------------

DataHandle* FamObjectName::dataHandle(const bool overwrite) const {
    return new FamHandle(*this, overwrite);
}

DataHandle* FamObjectName::partHandle(const OffsetList& offsets, const LengthList& lengths) const {
    return new FamHandle(*this, offsets[0], lengths[0], true);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
