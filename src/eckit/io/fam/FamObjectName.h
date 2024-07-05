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

/// @file   FamObjectName.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamObject.h"

#include <iosfwd>
#include <string>

namespace eckit {

class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class FamObjectName: public FamName {
public:  // methods
    using FamName::FamName;

    ~FamObjectName() = default;

    auto withRegion(std::string_view regionName) -> FamObjectName&;

    auto withObject(std::string_view objectName) -> FamObjectName&;

    auto lookup() const -> FamObject;

    auto allocate(fam::size_t objectSize, const bool overwrite = false) const -> FamObject;

    auto exists() const -> bool override;

    // data handles

    [[nodiscard]]
    auto dataHandle(bool overwrite = false) const -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const Offset& offset, const Length& length) const -> DataHandle*;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
