/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    RadosURIManager(const std::string& name);

    ~RadosURIManager() override;

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
