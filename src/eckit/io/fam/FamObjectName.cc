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

auto FamObjectName::withRegion(std::string_view regionName) -> FamObjectName& {
    path_.regionName = regionName;
    return *this;
}

auto FamObjectName::withObject(std::string_view objectName) -> FamObjectName& {
    path_.objectName = objectName;
    return *this;
}

auto FamObjectName::lookup() const -> FamObject {
    return session()->lookupObject(path_.regionName, path_.objectName);
}

auto FamObjectName::allocate(const fam::size_t objectSize, const bool overwrite) const -> FamObject {
    return session()->lookupRegion(path_.regionName).allocateObject(objectSize, path_.objectName, overwrite);
}

auto FamObjectName::exists() const -> bool {
    try {
        return lookup().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM object [" << path_.objectName << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM object [" << path_.objectName << "] is not accessible!\n";
    }
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
