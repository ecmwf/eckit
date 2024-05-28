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

#include "detail/FamObjectDetail.h"
#include "detail/FamRegionDetail.h"
#include "detail/FamSessionDetail.h"
#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/log/Log.h"

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// FACTORY

auto FamRegion::lookup(const std::string& name, const FamConfig& config) -> UPtr {
    auto session = FamSession::instance().getOrAdd(config);
    return std::make_unique<FamRegion>(session->lookupRegion(name));
}

auto FamRegion::create(const FamProperty& property, const FamConfig& config) -> UPtr {
    auto session = FamSession::instance().getOrAdd(config);
    return std::make_unique<FamRegion>(session->createRegion(property));
}

auto FamRegion::ensureCreated(const FamProperty& property, const FamConfig& config) -> UPtr {
    auto session = FamSession::instance().getOrAdd(config);
    try {
        return std::make_unique<FamRegion>(session->createRegion(property));
    } catch (const AlreadyExists& e) {
        Log::debug<LibEcKit>() << "Destroying region: " << property.name << '\n';
        session->lookupRegion(property.name)->destroy();
        return std::make_unique<FamRegion>(session->createRegion(property));
    }
}

//----------------------------------------------------------------------------------------------------------------------

FamRegion::FamRegion(std::unique_ptr<FamRegionDetail> region) noexcept: impl_(std::move(region)) { }

FamRegion::~FamRegion() = default;

//----------------------------------------------------------------------------------------------------------------------

void FamRegion::destroy() const {
    impl_->destroy();
}

auto FamRegion::exists() const -> bool {
    return (impl_->region()->get_desc_status() == Fam_Descriptor_Status::DESC_INIT_DONE);
}

auto FamRegion::name() const -> const char* {
    return impl_->name();
}

auto FamRegion::index() const -> uint64_t {
    return impl_->region()->get_global_descriptor().regionId;
}

auto FamRegion::size() const -> fam::size_t {
    return impl_->region()->get_size();
}

auto FamRegion::permissions() const -> fam::perm_t {
    return impl_->region()->get_perm();
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT factory methods

auto FamRegion::proxyObject(const fam::size_t offset) const -> FamObject::UPtr {
    return std::make_unique<FamObject>(impl_->proxyObject(offset));
}

auto FamRegion::lookupObject(const std::string& name) const -> FamObject::UPtr {
    return std::make_unique<FamObject>(impl_->lookupObject(name));
}

auto FamRegion::allocateObject(const FamProperty& property) const -> FamObject::UPtr {
    return std::make_unique<FamObject>(impl_->allocateObject(property));
}

auto FamRegion::allocateObject(const fam::size_t size, const std::string& name) const -> FamObject::UPtr {
    return std::make_unique<FamObject>(impl_->allocateObject(size, name));
}

void FamRegion::deallocateObject(const std::string& name) const {
    impl_->lookupObject(name)->deallocate();
}

auto FamRegion::ensureAllocatedObject(const FamProperty& property) const -> FamObject::UPtr {
    try {
        return allocateObject(property);
    } catch (const AlreadyExists& e) {
        Log::debug<LibEcKit>() << "Deallocating object: " << property.name << '\n';
        deallocateObject(property.name);
        return allocateObject(property);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamRegion::print(std::ostream& out) const {
    out << "FamRegion[" << property() << ",status=";
    switch (impl_->status()) {
        case Fam_Descriptor_Status::DESC_INVALID:                     out << "invalid"; break;
        case Fam_Descriptor_Status::DESC_INIT_DONE:                   out << "initialized"; break;
        case Fam_Descriptor_Status::DESC_INIT_DONE_BUT_KEY_NOT_VALID: out << "initialized_invalidkey"; break;
        case Fam_Descriptor_Status::DESC_UNINITIALIZED:               out << "uninitialized"; break;
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
