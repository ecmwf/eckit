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

/// @file   FamMapNode.h
/// @author Metin Cakircali
/// @date   Jul 2024

#pragma once

#include "FamNode.h"

#include <memory>

#include "eckit/io/fam/FamList.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FamMapNode : public FamNode {
    FamList::Descriptor desc;

    //------------------------------------------------------------------------------------------------------------------
    // HELPERS (DO NOT add any virtual function here)

    static FamList::Descriptor getDescriptor(const FamObject& object) {
        return object.get<FamList::Descriptor>(offsetof(FamMapNode, desc));
    }

    static std::unique_ptr<FamList> getList(const FamRegion& region, const FamObject& object) {
        return std::make_unique<FamList>(region, FamMapNode::getDescriptor(object));
    }
};

// FamMapNode is not standard-layout (data in both base and derived), but offsetof()
// is well-supported by all target compilers for trivially-copyable types.
static_assert(sizeof(FamMapNode) == 64, "FamMapNode layout changed — FAM on-wire format depends on this");
static_assert(offsetof(FamMapNode, desc) == 24, "FamMapNode::desc offset mismatch");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
