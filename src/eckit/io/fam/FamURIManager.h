// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamURIManager.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include <string>

#include "eckit/filesystem/URIManager.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamURIManager : public URIManager {
public:  // methods

    FamURIManager(const std::string& name);

    ~FamURIManager() override;

    bool authority() override { return true; }

private:  // methods

    bool exists(const URI& /*uri*/) override;

    DataHandle* newWriteHandle(const URI& uri) override;
    DataHandle* newReadHandle(const URI& uri) override;
    DataHandle* newReadHandle(const URI& uri, const OffsetList& offsets, const LengthList& lengths) override;

    std::string asString(const URI& uri) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
