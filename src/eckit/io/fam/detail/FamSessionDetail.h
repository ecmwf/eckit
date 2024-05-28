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

/// @file   FamSessionDetail.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamProperty.h"

#include <fam/fam.h>

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

using FamGlobalDescriptor = Fam_Global_Descriptor;
using FamObjectDescriptor = openfam::Fam_Descriptor;
using FamRegionDescriptor = openfam::Fam_Region_Descriptor;

class FamObjectDetail;
class FamRegionDetail;

//----------------------------------------------------------------------------------------------------------------------
// SESSION

class FamSessionDetail: public std::enable_shared_from_this<FamSessionDetail> {
public:  // methods
    FamSessionDetail(FamConfig config);

    FamSessionDetail(const FamSessionDetail&)            = delete;
    FamSessionDetail& operator=(const FamSessionDetail&) = delete;
    FamSessionDetail(FamSessionDetail&&)                 = delete;
    FamSessionDetail& operator=(FamSessionDetail&&)      = delete;

    ~FamSessionDetail();

    auto getShared() -> std::shared_ptr<FamSessionDetail> { return shared_from_this(); }

    auto config() const -> const FamConfig& { return config_; }

    //------------------------------------------------------------------------------------------------------------------
    // REGION

    auto lookupRegion(const std::string& name) -> std::unique_ptr<FamRegionDetail>;

    auto createRegion(const FamProperty& property) -> std::unique_ptr<FamRegionDetail>;

    void resizeRegion(FamRegionDescriptor& region, fam::size_t size);

    void destroyRegion(FamRegionDescriptor& region);

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT

    auto proxyObject(const FamGlobalDescriptor& descriptor) -> std::unique_ptr<FamObjectDetail>;

    auto proxyObject(const FamGlobalDescriptor& descriptor, fam::size_t size) -> std::unique_ptr<FamObjectDetail>;

    auto lookupObject(const std::string& regionName, const std::string& objectName) -> std::unique_ptr<FamObjectDetail>;

    auto allocateObject(FamRegionDescriptor& region, const FamProperty& property) -> std::unique_ptr<FamObjectDetail>;

    void deallocateObject(FamObjectDescriptor& object);

    void statObject(const FamObjectDescriptor& object, Fam_Stat* info) const;

    void put(FamObjectDescriptor& object, const void* buffer, fam::size_t offset, fam::size_t length);

    void get(FamObjectDescriptor& object, void* buffer, fam::size_t offset, fam::size_t length);

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT - ATOMIC

    template<typename T>
    auto fetch(FamObjectDescriptor& object, fam::size_t offset) -> T;

    template<typename T>
    void set(FamObjectDescriptor& object, fam::size_t offset, T value);

    template<typename T>
    void add(FamObjectDescriptor& object, fam::size_t offset, T value);

    template<typename T>
    auto swap(FamObjectDescriptor& object, fam::size_t offset, T value) -> T;

    template<typename T>
    auto compareSwap(FamObjectDescriptor& object, fam::size_t offset, T oldValue, T newValue) -> T;

    //------------------------------------------------------------------------------------------------------------------

private:  // methods
    friend std::ostream& operator<<(std::ostream& out, const FamSessionDetail& session);

    void print(std::ostream& out) const;

private:  // members
    const FamConfig config_;

    openfam::fam fam_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
