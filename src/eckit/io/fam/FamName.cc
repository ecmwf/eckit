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

namespace {

auto parsePath(const std::string& path) -> std::tuple<std::string, std::string> {
    const auto names = Tokenizer("/").tokenize(path);
    switch (names.size()) {
        case 1:  return {names[0], ""}; break;
        case 2:  return {names[0], names[1]}; break;
        default: return {}; break;
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

FamNamePath::FamNamePath(const std::string& path) {
    std::tie(region, object) = parsePath(path);
}

bool FamNamePath::operator==(const FamNamePath& other) const {
    return (region == other.region && object == other.object);
}

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(FamSession::SPtr session, FamNamePath path) noexcept:
    session_ {std::move(session)}, path_ {std::move(path)} { }

FamName::FamName(const net::Endpoint& endpoint, FamNamePath path):
    FamName(FamSession::instance().getOrAdd({endpoint}), std::move(path)) { }

FamName::FamName(const URI& uri): FamName(uri, uri.name()) {
    ASSERT(uri.scheme() == SCHEME);
}

FamName::~FamName() = default;

// bool FamName::operator==(const FamName& other) const {
//     return (config().endpoint == other.config().endpoint && path_ == other.path_);
// }
//
//----------------------------------------------------------------------------------------------------------------------

auto FamName::asString() const -> std::string {
    std::string retVal = std::string(config().endpoint);

    if (path_.region.empty()) { return retVal; }

    retVal += '/' + path_.region;

    if (!path_.object.empty()) { retVal += '/' + path_.object; }

    return retVal;
}

auto FamName::uri() const -> URI {
    return {SCHEME, asString()};
}

auto FamName::with(std::string_view regionName) -> FamName& {
    path_.region = regionName;
    return *this;
}

auto FamName::with(std::string_view regionName, std::string_view objectName) -> FamName& {
    path_.region = regionName;
    path_.object = objectName;
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
// REGION

auto FamName::lookupRegion() const -> FamRegion {
    return session_->lookupRegion(path_.region);
}

auto FamName::createRegion(const fam::size_t regionSize,
                           const fam::perm_t regionPerm,
                           const bool        overwrite) const -> FamRegion {
    if (overwrite) { return session_->ensureCreateRegion(regionSize, regionPerm, path_.region); }
    return session_->createRegion(regionSize, regionPerm, path_.region);
}

auto FamName::existsRegion() const -> bool {
    try {
        return lookupRegion().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM region [" << path_.region << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM region [" << path_.region << "] is not accessible!\n";
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

auto FamName::lookupObject() const -> FamObject {
    return session_->lookupObject(path_.region, path_.object);
}

auto FamName::allocateObject(const fam::size_t objectSize, const bool overwrite) const -> FamObject {
    return lookupRegion().allocateObject(objectSize, path_.object, overwrite);
}

auto FamName::existsObject() const -> bool {
    try {
        return lookupObject().exists();
    } catch (const NotFound&) {
        Log::debug<LibEcKit>() << "FAM object [" << path_.object << "] was not found!\n";
    } catch (const PermissionDenied&) {
        Log::debug<LibEcKit>() << "FAM object [" << path_.object << "] is not accessible!\n";
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

void FamName::print(std::ostream& out) const {
    out << session_ << ", region=" << path_.region << ", object=" << path_.object;
}

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
