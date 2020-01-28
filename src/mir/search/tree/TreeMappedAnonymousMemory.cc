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


#include "mir/search/tree/TreeMappedAnonymousMemory.h"


namespace mir {
namespace search {
namespace tree {


TreeMappedAnonymousMemory::TreeMappedAnonymousMemory(const repres::Representation& r,
                                                     const param::MIRParametrisation&) :
    TreeMapped(r, "/dev/zero") {}


static TreeBuilder<TreeMappedAnonymousMemory> builder("mapped-anonymous-memory");


}  // namespace tree
}  // namespace search
}  // namespace mir
