// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/search/Tree.h"
#include "eckit/geo/search/TreeMapped.h"


namespace eckit::geo::search {


class TreeMappedAnonymousMemory : public TreeMapped {
    bool ready() const override { return false; }
    void commit() override {}
    void print(std::ostream& out) const override { out << "TreeMappedAnonymousMemory[]"; }

public:

    explicit TreeMappedAnonymousMemory(const Grid&);
};


}  // namespace eckit::geo::search
