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


#pragma once

#include "mir/search/tree/TreeMapped.h"


namespace mir::search::tree {


class TreeMappedAnonymousMemory : public TreeMapped {

    bool ready() const override { return false; }

    void commit() override {}

    void print(std::ostream& out) const override { out << "TreeMappedAnonymousMemory[]"; }

public:
    TreeMappedAnonymousMemory(const repres::Representation&);
};


}  // namespace mir::search::tree
