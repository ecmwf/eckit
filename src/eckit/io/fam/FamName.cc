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
#include "eckit/utils/Tokenizer.h"

namespace eckit {

constexpr const auto scheme = "fam";

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(FamSession::SPtr session, const std::string& path) noexcept: session_ {std::move(session)} {
    parsePath(path);
}

FamName::FamName(const URI& uri): FamName(FamSession::instance().getOrAdd({uri}), uri.name()) {
    ASSERT(uri.scheme() == scheme);
}

FamName::FamName(const std::string& uri): FamName(URI {uri}) { }

FamName::~FamName() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamName::asString() const -> std::string {
    return std::string(config().endpoint) + '/' + regionName_ + '/' + objectName_;
}

auto FamName::uri() const -> URI {
    return {scheme, asString()};
}

auto FamName::with(std::string_view regionName) -> FamName& {
    regionName_ = regionName;
    return *this;
}

auto FamName::with(std::string_view regionName, std::string_view objectName) -> FamName& {
    regionName_ = regionName;
    objectName_ = objectName;
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
// REGION

auto FamName::lookupRegion() const -> FamRegion {
    return session_->lookupRegion(regionName_);
}

auto FamName::createRegion(const fam::size_t regionSize,
                           const fam::perm_t regionPerm,
                           const bool        overwrite) const -> FamRegion {
    if (overwrite) { return session_->ensureCreateRegion(regionSize, regionPerm, regionName_); }
    return session_->createRegion(regionSize, regionPerm, regionName_);
}

auto FamName::existsRegion() const -> bool {
    try {
        return lookupRegion().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM region [" << regionName_ << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM region [" << regionName_ << "] is not accessible!\n";
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

auto FamName::lookupObject() const -> FamObject {
    return session_->lookupObject(regionName_, objectName_);
}

auto FamName::allocateObject(const fam::size_t objectSize, const bool overwrite) const -> FamObject {
    return lookupRegion().allocateObject(objectSize, objectName_, overwrite);
}

auto FamName::existsObject() const -> bool {
    try {
        return lookupObject().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM object [" << objectName_ << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM object [" << objectName_ << "] is not accessible!\n";
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto FamName::dataHandle(const bool overwrite) const -> DataHandle* {
    return new FamHandle(asString(), overwrite);
}

auto FamName::dataHandle(const Offset& offset, const Length& length) const -> DataHandle* {
    return new FamHandle(asString(), offset, length, true);
}

//----------------------------------------------------------------------------------------------------------------------

auto FamName::config() const -> const FamConfig& {
    return session_->config();
}

void FamName::parsePath(const std::string& path) {
    const auto names = Tokenizer("/").tokenize(path);
    const auto size  = names.size();
    if (size > 0) { regionName_ = names[0]; }
    if (size > 1) { objectName_ = names[1]; }
}

void FamName::print(std::ostream& out) const {
    out << session_ << ", region=" << regionName_ << ", object=" << objectName_;
}

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
