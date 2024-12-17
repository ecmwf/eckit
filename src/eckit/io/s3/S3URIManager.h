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
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

/// @file   S3URIManager.h
/// @author Simon Smart
/// @author Metin Cakircali
/// @date   Feb 2024

#pragma once

#include "eckit/filesystem/URIManager.h"

#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3URIManager : public URIManager {
public:  // methods
    explicit S3URIManager(const std::string& name);

    bool authority() override { return true; }

private:  // methods
    bool exists(const URI& uri) override;

    DataHandle* newWriteHandle(const URI& uri) override;
    DataHandle* newReadHandle(const URI& uri) override;
    DataHandle* newReadHandle(const URI& uri, const OffsetList& offsets, const LengthList& lengths) override;

    std::string asString(const URI& uri) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
