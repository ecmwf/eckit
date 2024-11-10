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

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamHandle.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/log/Log.h"

#include <ostream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto FamObjectName::withRegion(const std::string& regionName) -> FamObjectName& {
    path().regionName = regionName;
    return *this;
}

auto FamObjectName::withObject(const std::string& objectName) -> FamObjectName& {
    path().objectName = objectName;
    return *this;
}

auto FamObjectName::withUUID() -> FamObjectName& {
    return withObject(path().generateUUID());
}

auto FamObjectName::lookup() const -> FamObject {
    return session()->lookupObject(path().regionName, path().objectName);
}

auto FamObjectName::allocate(const fam::size_t objectSize, const bool overwrite) const -> FamObject {
    return session()->lookupRegion(path().regionName).allocateObject(objectSize, path().objectName, overwrite);
}

auto FamObjectName::exists() const -> bool {
    try {
        return lookup().exists();
    } catch (const NotFound& notFound) {
        Log::debug<LibEcKit>() << notFound << '\n';
    } catch (const PermissionDenied& permissionDenied) { Log::debug<LibEcKit>() << permissionDenied << '\n'; }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto FamObjectName::dataHandle(const bool overwrite) const -> DataHandle* {
    return new FamHandle(*this, overwrite);
}

auto FamObjectName::dataHandle(const Offset& offset, const Length& length) const -> DataHandle* {
    return new FamHandle(*this, offset, length, true);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
