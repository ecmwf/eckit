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

FamName::FamName(FamSession::SPtr session, FamPath path) noexcept:
    session_ {std::move(session)}, path_ {std::move(path)} { }

FamName::FamName(const net::Endpoint& endpoint, FamPath path):
    FamName(FamSession::instance().getOrAdd({endpoint}), std::move(path)) { }

FamName::FamName(const URI& uri): FamName(uri.endpoint(), uri) { }

//----------------------------------------------------------------------------------------------------------------------

auto FamName::asString() const -> std::string {
    std::ostringstream oss;
    oss << FamPath::SCHEME << "://" << config().endpoint << path_;
    return oss.str();
}

auto FamName::uri() const -> URI {
    // return URI {asString()};
    return {FamPath::SCHEME, config().endpoint, path_.asString()};
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
    return session_->lookupRegion(path_.regionName);
}

auto FamName::createRegion(const fam::size_t regionSize,
                           const fam::perm_t regionPerm,
                           const bool        overwrite) const -> FamRegion {
    if (overwrite) { return session_->ensureCreateRegion(regionSize, regionPerm, path_.regionName); }
    return session_->createRegion(regionSize, regionPerm, path_.regionName);
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
    return session_->lookupObject(path_.regionName, path_.objectName);
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

auto FamName::config() const -> const FamConfig& {
    return session_->config();
}

void FamName::print(std::ostream& out) const {
    out << session_ << ", path=" << path_;
}

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
