// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeMappedAnonymousMemory.h"


namespace eckit::geo::search {


TreeMappedAnonymousMemory::TreeMappedAnonymousMemory(const Grid& r) : TreeMapped(r, "/dev/zero") {}


static const TreeBuilder<TreeMappedAnonymousMemory> builder("mapped-anonymous-memory");


}  // namespace eckit::geo::search
