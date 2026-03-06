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

#include <string>

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamProperty.h"

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

    FamObject allocate(fam::size_t object_size, bool overwrite = false) const;

    bool exists() const override;

    // data handles

    [[nodiscard]]
    DataHandle* dataHandle(bool overwrite = false) const;

    [[nodiscard]]
    DataHandle* partHandle(const OffsetList& offsets, const LengthList& lengths) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
