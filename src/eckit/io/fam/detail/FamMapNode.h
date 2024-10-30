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
#include "eckit/io/fam/FamList.h"

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FamMapNode: public FamNode {
    FamList::Descriptor desc;

    //------------------------------------------------------------------------------------------------------------------
    // HELPERS (DO NOT add any virtual function here)

    static auto getDescriptor(const FamObject& object) -> FamList::Descriptor {
        return object.get<FamList::Descriptor>(offsetof(FamMapNode, desc));
    }

    static auto getList(const FamRegion& region, const FamObject& object) -> std::unique_ptr<FamList> {
        return std::make_unique<FamList>(region, FamMapNode::getDescriptor(object));
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
