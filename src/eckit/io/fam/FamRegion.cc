// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamRegion.h"

#include <memory>
#include <ostream>
#include <sstream>
#include <string>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/io/fam/FamTypes.h"
#include "eckit/log/Log.h"

#include "fam/fam.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamRegion::FamRegion(FamSession& session, FamRegionDescriptor* region) :
    session_{session.shared_from_this()}, region_{region} {
    ASSERT(region_);
}

//----------------------------------------------------------------------------------------------------------------------

void FamRegion::destroy() const {
    session_->destroyRegion(*region_);
}

bool FamRegion::exists() const {
    return (region_->get_desc_status() != openfam::Fam_Descriptor_Status::DESC_INVALID);
}

//----------------------------------------------------------------------------------------------------------------------
// PROPERTIES

fam::index_t FamRegion::index() const {
    return region_->get_global_descriptor().regionId;
}

fam::index_t FamRegion::objectIndex() const {
    const auto observed = *objectIndex_;
    return observed != 0 ? observed : index();
}

void FamRegion::noteObjectIndex(const FamObject& object) const {
    useObjectIndex(object.regionId());
}

void FamRegion::useObjectIndex(const fam::index_t observed) const {
    if (observed == 0 || *objectIndex_ == observed) {
        return;
    }
    if (observed != index()) {
        LOG_DEBUG_LIB(LibEcKit) << "FAM region " << name() << " reports regionId=" << index()
                                << " but its objects carry regionId=" << observed
                                << "; proxying objects with the latter\n";
    }
    *objectIndex_ = observed;
}

fam::size_t FamRegion::size() const {
    return region_->get_size();
}

fam::perm_t FamRegion::permissions() const {
    return region_->get_perm();
}

std::string FamRegion::name() const {
    return region_->get_name() ? region_->get_name() : "";
}

FamProperty FamRegion::property() const {
    return {size(), permissions(), name()};
}

void FamRegion::setRegionLevelPermissions() const {
    region_->set_permissionLevel(Fam_Permission_Level::REGION);
}

void FamRegion::setObjectLevelPermissions() const {
    region_->set_permissionLevel(Fam_Permission_Level::DATAITEM);
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT factory methods

// Creates a FamObject wrapper around an existing object identified by {regionId, offset}
FamObject FamRegion::proxyObject(const fam::index_t offset) const {
    return session_->proxyObject(objectIndex(), offset);
}

FamObject FamRegion::lookupObject(const std::string& object_name) const {
    auto object = session_->lookupObject(name(), object_name);
    noteObjectIndex(object);
    return object;
}

FamObject FamRegion::allocateObject(const fam::size_t object_size, const fam::perm_t object_perm,
                                    const std::string& object_name, const bool overwrite) const {
    auto object = overwrite ? session_->ensureAllocateObject(*region_, object_size, object_perm, object_name)
                            : session_->allocateObject(*region_, object_size, object_perm, object_name);
    noteObjectIndex(object);
    return object;
}

void FamRegion::deallocateObject(const std::string& object_name) const {
    session_->deallocateObject(region_->get_name(), object_name);
}

FamObject FamRegion::ensureObject(const fam::size_t object_size, const std::string& object_name) const {
    try {
        return allocateObject(object_size, object_name);
    }
    catch (const AlreadyExists&) {
        auto object = lookupObject(object_name);
        if (object.size() != object_size) {
            std::ostringstream msg;
            msg << "FamRegion::ensureObject: size mismatch for object '" << object_name << "' in region '"
                << region_->get_name() << "': existing=" << object.size() << ", requested=" << object_size;
            throw SeriousBug(msg.str(), Here());
        }
        return object;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamRegion::print(std::ostream& out) const {
    out << "FamRegion[" << property() << ",status=";
    switch (region_->get_desc_status()) {
        case openfam::Fam_Descriptor_Status::DESC_INVALID:
            out << "invalid";
            break;
        case openfam::Fam_Descriptor_Status::DESC_INIT_DONE:
            out << "initialized";
            break;
        case openfam::Fam_Descriptor_Status::DESC_INIT_DONE_BUT_KEY_NOT_VALID:
            out << "initialized_invalidkey";
            break;
        case openfam::Fam_Descriptor_Status::DESC_UNINITIALIZED:
            out << "uninitialized";
            break;
        default:
            out << "unknown";
            break;
    }
    out << ",perm. Level=";
    switch (region_->get_permissionLevel()) {
        case Fam_Permission_Level::REGION:
            out << "region";
            break;
        case Fam_Permission_Level::DATAITEM:
            out << "dataitem";
            break;
        case Fam_Permission_Level::PERMISSION_LEVEL_DEFAULT:
            out << "default";
            break;
        default:
            out << "unknown";
            break;
    }
    out << "]";
}

std::ostream& operator<<(std::ostream& out, const FamRegion& region) {
    region.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
