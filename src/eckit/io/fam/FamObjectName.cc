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
#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamHandle.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/io/fam/detail/FamObjectDetail.h"
#include "eckit/io/fam/detail/FamRegionDetail.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

void validateName(const std::string& name) {
    if (!name.empty()) { return; }
    throw UserError("Invalid name!", Here());
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

FamObjectName::FamObjectName(const URI& uri): FamName(uri) {
    const auto pairs = parseName();

    if (pairs.empty()) { throw UserError("Could not parse region/object names!", Here()); }

    regionName_ = pairs[0];
    validateName(regionName_);

    if (pairs.size() > 1) { objectName_ = pairs[1]; }
}

FamObjectName::FamObjectName(const net::Endpoint& endpoint, std::string regionName, std::string objectName):
    FamName(endpoint, "/"), regionName_ {std::move(regionName)}, objectName_ {std::move(objectName)} {
    validateName(regionName_);
}

//----------------------------------------------------------------------------------------------------------------------

auto FamObjectName::lookup() -> bool {
    if (!object_) {
        validateName(objectName_);
        try {
            object_ = session()->lookupObject(regionName_, objectName_);
        } catch (const NotFound&) {
            Log::debug<LibEcKit>() << "Object is not found!\n";
        } catch (const PermissionDenied&) { Log::debug<LibEcKit>() << "Object is not accessible!\n"; }
    }
    return (object_ != nullptr);
}

void FamObjectName::create(const FamProperty& property) {
    objectName_ = property.name;
    validateName(objectName_);
    object_ = session()->lookupRegion(regionName_)->allocateObject(property);
}

auto FamObjectName::exists() -> bool {
    return (object_ || lookup());
}

auto FamObjectName::size() const -> fam::size_t {
    ASSERT(object_);
    return object_->size();
}

void FamObjectName::destroy() {
    ASSERT(object_);
    object_->deallocate();
}

void FamObjectName::put(const void* buffer, const fam::size_t offset, const fam::size_t length) const {
    ASSERT(object_);
    object_->put(buffer, offset, length);
}

void FamObjectName::get(void* buffer, const fam::size_t offset, const fam::size_t length) const {
    ASSERT(object_);
    object_->get(buffer, offset, length);
}

auto FamObjectName::dataHandle(const Offset& offset) const -> DataHandle* {
    return new FamHandle(*this, offset);
}

auto FamObjectName::dataHandle(const Offset& offset, const Length& length) const -> DataHandle* {
    return new FamHandle(*this, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------

auto FamObjectName::asString() const -> std::string {
    auto result = std::string(FamName::asString() + '/' + regionName_);

    if (!objectName_.empty()) { result += '/' + objectName_; }

    return result;
}

void FamObjectName::print(std::ostream& out) const {
    out << "FamObjectName[object=" << objectName_ << ",region=" << regionName_;
    FamName::print(out);
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
