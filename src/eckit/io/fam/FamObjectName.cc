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

FamObjectName::FamObjectName(const URI& uri): FamName(uri) {
    const auto pairs = parseName();
    if (pairs.empty()) { throw UserError("Could not parse region/object names!", Here()); }
    regionName_ = pairs[0];
    if (pairs.size() > 1) { objectName_ = pairs[1]; }
}

FamObjectName::FamObjectName(const net::Endpoint& endpoint, std::string regionName, std::string objectName):
    FamName(endpoint, "/"), regionName_ {std::move(regionName)}, objectName_ {std::move(objectName)} { }

//----------------------------------------------------------------------------------------------------------------------

auto FamObjectName::exists() const -> bool {
    return (object_ != nullptr);
}

void FamObjectName::lookup() {
    if (object_) { return; }
    validateNames();
    try {
        object_ = session()->lookupObject(regionName_, objectName_);
    } catch (const NotFound&) { Log::debug<LibEcKit>() << "Object could not be found!\n"; }
}

void FamObjectName::create(const FamProperty& property) {
    objectName_ = property.name;
    validateNames();
    object_ = session()->lookupRegion(regionName_)->allocateObject(property);
}

void FamObjectName::destroy() {
    lookup();
    if (object_) { object_->deallocate(); }
    object_.reset();
}

void FamObjectName::put(const void* buffer, const fam::size_t offset, const fam::size_t length) const {
    ASSERT(object_);
    object_->put(buffer, offset, length);
}

void FamObjectName::get(void* buffer, const fam::size_t offset, const fam::size_t length) const {
    ASSERT(object_);
    object_->get(buffer, offset, length);
}

auto FamObjectName::dataHandle() const -> DataHandle* {
    return new FamHandle(*this);
}

auto FamObjectName::dataHandle(const Offset& offset, const Length& length) const -> DataHandle* {
    return new FamHandle(*this, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------

auto FamObjectName::size() const -> fam::size_t {
    return object_ ? object_->object()->get_size() : 0;
}

void FamObjectName::validateNames() const {
    if (regionName_.empty() || objectName_.empty()) {
        throw UserError("Region and object names must not be empty!", Here());
    }
}

auto FamObjectName::asString() const -> std::string {
    return FamName::asString() + "/" + regionName_ + "/" + objectName_;
}

void FamObjectName::print(std::ostream& out) const {
    out << "FamObjectName[object=" << objectName_ << ",region=" << regionName_;
    FamName::print(out);
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
