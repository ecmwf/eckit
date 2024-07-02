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

#include "eckit/io/fam/FamName.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamHandle.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/log/Log.h"

#include <iostream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(const net::Endpoint& endpoint, FamPath path): endpoint_ {endpoint}, path_ {std::move(path)} { }

FamName::FamName(const URI& uri): FamName(uri.endpoint(), uri) { }

//----------------------------------------------------------------------------------------------------------------------

auto FamName::session() const -> FamSession::SPtr {
    return FamSession::instance().getOrAdd({endpoint_});
}

auto FamName::asString() const -> std::string {
    std::ostringstream oss;
    oss << FamPath::SCHEME << "://" << endpoint_ << path_;
    return oss.str();
}

auto FamName::uri() const -> URI {
    return {FamPath::SCHEME, endpoint_, path_};
}

auto FamName::withEndpoint(const net::Endpoint& endpoint) -> FamName& {
    endpoint_ = endpoint;
    return *this;
}

auto FamName::withPath(const FamPath& path) -> FamName& {
    path_ = path;
    return *this;
}

auto FamName::withRegion(std::string_view regionName) -> FamName& {
    path_.regionName = regionName;
    return *this;
}

auto FamName::withObject(std::string_view objectName) -> FamName& {
    path_.objectName = objectName;
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
// REGION

auto FamName::lookupRegion() const -> FamRegion {
    return session()->lookupRegion(path_.regionName);
}

auto FamName::createRegion(const fam::size_t regionSize,
                           const fam::perm_t regionPerm,
                           const bool        overwrite) const -> FamRegion {
    if (overwrite) { return session()->ensureCreateRegion(regionSize, regionPerm, path_.regionName); }
    return session()->createRegion(regionSize, regionPerm, path_.regionName);
}

auto FamName::existsRegion() const -> bool {
    try {
        return lookupRegion().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM region [" << path_.regionName << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM region [" << path_.regionName << "] is not accessible!\n";
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

auto FamName::lookupObject() const -> FamObject {
    return session()->lookupObject(path_.regionName, path_.objectName);
}

auto FamName::allocateObject(const fam::size_t objectSize, const bool overwrite) const -> FamObject {
    return lookupRegion().allocateObject(objectSize, path_.objectName, overwrite);
}

auto FamName::existsObject() const -> bool {
    try {
        return lookupObject().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM object [" << path_.objectName << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM object [" << path_.objectName << "] is not accessible!\n";
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto FamName::dataHandle(const bool overwrite) const -> DataHandle* {
    return new FamHandle(*this, overwrite);
}

auto FamName::dataHandle(const Offset& offset, const Length& length) const -> DataHandle* {
    return new FamHandle(*this, offset, length, true);
}

//----------------------------------------------------------------------------------------------------------------------

void FamName::print(std::ostream& out) const {
    out << "endpoint=" << endpoint_ << ", path=" << path_;
}

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
