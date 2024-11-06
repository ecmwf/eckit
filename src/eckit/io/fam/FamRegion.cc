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

#include "eckit/io/fam/FamRegion.h"

#include "detail/FamSessionDetail.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamObject.h"

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamRegion::FamRegion(FamSessionDetail& session, std::unique_ptr<FamRegionDescriptor> region):
    session_ {session.getShared()}, region_(std::move(region)) {
    ASSERT(region_);
}

FamRegion::~FamRegion() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamRegion::clone() const -> UPtr {
    auto clone = std::make_unique<FamRegionDescriptor>(region_->get_global_descriptor());
    clone->set_size(region_->get_size());
    clone->set_perm(region_->get_perm());
    clone->set_name(region_->get_name());
    clone->set_desc_status(region_->get_desc_status());
    clone->set_redundancyLevel(region_->get_redundancyLevel());
    clone->set_memoryType(region_->get_memoryType());
    clone->set_interleaveEnable(region_->get_interleaveEnable());
    return std::make_unique<FamRegion>(*session_, std::move(clone));
}

void FamRegion::destroy() const {
    session_->destroyRegion(*region_);
}

auto FamRegion::exists() const -> bool {
    return (region_->get_desc_status() != FamDescriptorStatus::DESC_INVALID);
}

//----------------------------------------------------------------------------------------------------------------------
// PROPERTIES

auto FamRegion::index() const -> fam::index_t {
    return region_->get_global_descriptor().regionId;
}

auto FamRegion::size() const -> fam::size_t {
    return region_->get_size();
}

auto FamRegion::permissions() const -> fam::perm_t {
    return region_->get_perm();
}

auto FamRegion::name() const -> std::string {
    return region_->get_name() ? region_->get_name() : "";
}

auto FamRegion::property() const -> FamProperty {
    return {size(), permissions(), name()};
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT factory methods

auto FamRegion::proxyObject(const fam::index_t offset) const -> FamObject {
    return session_->proxyObject(index(), offset);
}

auto FamRegion::lookupObject(const std::string& objectName) const -> FamObject {
    return session_->lookupObject(name(), objectName);
}

auto FamRegion::allocateObject(const fam::size_t  objectSize,
                               const fam::perm_t  objectPerm,
                               const std::string& objectName,
                               const bool         overwrite) const -> FamObject {
    if (overwrite) { return session_->ensureAllocateObject(*region_, objectSize, objectPerm, objectName); }
    return session_->allocateObject(*region_, objectSize, objectPerm, objectName);
}

void FamRegion::deallocateObject(const std::string& objectName) const {
    session_->deallocateObject(region_->get_name(), objectName);
}

//----------------------------------------------------------------------------------------------------------------------

void FamRegion::print(std::ostream& out) const {
    out << "FamRegion[" << property() << ",status=";
    switch (region_->get_desc_status()) {
        case FamDescriptorStatus::DESC_INVALID:                       out << "invalid"; break;
        case FamDescriptorStatus::DESC_INIT_DONE:                     out << "initialized"; break;
        case FamDescriptorStatus::DESC_INIT_DONE_BUT_KEY_NOT_VALID:   out << "initialized_invalidkey"; break;
        case FamDescriptorStatus::DESC_UNINITIALIZED:                 out << "uninitialized"; break;
        default:                                                      out << "unknown"; break;
    }
    out << "]";
}

std::ostream& operator<<(std::ostream& out, const FamRegion& region) {
    region.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
