// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamObjectName.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include <optional>
#include <string>

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamTypes.h"

namespace eckit {

class DataHandle;
class FamObject;

//----------------------------------------------------------------------------------------------------------------------

class FamObjectName : public FamName {
public:  // methods

    using FamName::FamName;

    FamObjectName& withObject(const std::string& object_name);

    /// @brief Replaces [objectName] with UUID (e.g., 34bd2214-2a97-5a8a-802f-76ebefd84816)
    FamObjectName& withUUID();

    FamObject lookup() const;

    /// Allocates the named object. Without @p object_perm the object inherits the region's permissions.
    FamObject allocate(fam::size_t object_size, bool overwrite = false,
                       std::optional<fam::perm_t> object_perm = {}) const;

    bool exists() const override;

    // data handles

    [[nodiscard]]
    DataHandle* dataHandle(bool overwrite = false) const;

    [[nodiscard]]
    DataHandle* partHandle(const OffsetList& offsets, const LengthList& lengths) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
