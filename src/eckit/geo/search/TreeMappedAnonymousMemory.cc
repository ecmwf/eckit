/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/search/TreeMappedAnonymousMemory.h"


namespace eckit::geo::search {


TreeMappedAnonymousMemory::TreeMappedAnonymousMemory(const Grid& r) : TreeMapped(r, "/dev/zero") {}


static const TreeBuilder<TreeMappedAnonymousMemory> builder("mapped-anonymous-memory");


}  // namespace eckit::geo::search
