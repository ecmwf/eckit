// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RadosURIManager.h

#pragma once

#include <string>

#include "eckit/filesystem/URIManager.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosURIManager : public URIManager {
public:  // methods

    explicit RadosURIManager(const std::string& name);

private:  // methods

    bool query() override { return true; }
    bool fragment() override { return true; }

    bool exists(const URI& uri) override;

    DataHandle* newWriteHandle(const URI& uri) override;
    DataHandle* newReadHandle(const URI& uri) override;
    DataHandle* newReadHandle(const URI& uri, const OffsetList& offsets, const LengthList& lengths) override;

    std::string asString(const URI& uri) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
