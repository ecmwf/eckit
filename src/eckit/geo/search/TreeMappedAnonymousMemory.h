// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/search/TreeMapped.h"


namespace eckit::geo::search {


/// Mapping /dev/zero, or an unlinked temporary file where not supported
class TreeMappedAnonymousMemory final : public TreeMapped {
public:

    // -- Constructors

    TreeMappedAnonymousMemory(const std::string& uid, size_t size);

    // -- Overridden methods

    bool ready() override { return false; }
    void commit() override {}

private:

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
