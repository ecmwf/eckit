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

/// @file   FamRegionDetail.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "FamSessionDetail.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamProperty.h"

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamRegionDetail {
public:  // methods
    FamRegionDetail(FamSessionDetail& session, std::unique_ptr<FamRegionDescriptor> region):
        session_ {session.getShared()}, region_ {std::move(region)} {
        ASSERT(region_);
    }

    auto region() -> FamRegionDescriptor* { return region_.get(); }

    auto region() const -> const FamRegionDescriptor* { return region_.get(); }

    auto name() const -> const char* { return region_->get_name() ? region_->get_name() : ""; }

    auto status() const -> int { return region_->get_desc_status(); }

    void destroy() { session_->destroyRegion(*region_); }

    // OBJECT methods

    [[nodiscard]]
    auto proxyObject(const fam::size_t offset) -> std::unique_ptr<FamObjectDetail> {
        return session_->proxyObject({region_->get_global_descriptor().regionId, offset});
    }

    [[nodiscard]]
    auto lookupObject(const std::string& name) -> std::unique_ptr<FamObjectDetail> {
        return session_->lookupObject(region_->get_name(), name);
    }

    [[nodiscard]]
    auto allocateObject(const FamProperty& property) -> std::unique_ptr<FamObjectDetail> {
        return session_->allocateObject(*region_, property);
    }

    [[nodiscard]]
    /// @note permissions are inherited from region
    auto allocateObject(const fam::size_t size, const std::string& name = "") -> std::unique_ptr<FamObjectDetail> {
        return allocateObject({size, region_->get_perm(), name});
    }

private:  // members
    std::shared_ptr<FamSessionDetail> session_;

    // openfam region descriptor
    std::unique_ptr<FamRegionDescriptor> region_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
