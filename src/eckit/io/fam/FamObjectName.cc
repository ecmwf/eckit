// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamObjectName.h"

#include <optional>
#include <ostream>
#include <string>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamHandle.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/io/fam/FamTypes.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamObjectName& FamObjectName::withObject(const std::string& object_name) {
    path().objectName(object_name);
    return *this;
}

FamObjectName& FamObjectName::withUUID() {
    return withObject(path().generateUUID());
}

FamObject FamObjectName::lookup() const {
    return session()->lookupObject(path().regionName(), path().objectName());
}

FamObject FamObjectName::allocate(const fam::size_t object_size, const bool overwrite,
                                  const std::optional<fam::perm_t> object_perm) const {
    const auto region = session()->lookupRegion(path().regionName());
    return region.allocateObject(object_size, object_perm.value_or(region.permissions()), path().objectName(),
                                 overwrite);
}

bool FamObjectName::exists() const {
    try {
        return lookup().exists();
    }
    catch (const NotFound& not_found) {
        LOG_DEBUG_LIB(LibEcKit) << not_found << '\n';
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

DataHandle* FamObjectName::dataHandle(const bool overwrite) const {
    return new FamHandle(*this, overwrite);
}

DataHandle* FamObjectName::partHandle(const OffsetList& offsets, const LengthList& lengths) const {
    ASSERT(!offsets.empty() && !lengths.empty());
    // FAM objects are single contiguous allocations — only the first part is supported.
    return new FamHandle(*this, offsets[0], lengths[0], true);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
