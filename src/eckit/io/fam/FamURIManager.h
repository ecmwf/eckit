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
