// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

struct FamMapNode {
    FamNode header{};          // 16 bytes: { version, next }
    FamList::Descriptor desc;  // bucket FamList descriptor

    //------------------------------------------------------------------------------------------------------------------
    // HELPERS (DO NOT add any virtual function here)

    static FamList::Descriptor getDescriptor(const FamObject& object) {
        return object.get<FamList::Descriptor>(offsetof(FamMapNode, desc));
    }

    static std::unique_ptr<FamList> getList(const FamRegion& region, const FamObject& object) {
        return std::make_unique<FamList>(region, FamMapNode::getDescriptor(object));
    }
};

static_assert(std::is_standard_layout_v<FamMapNode>, "FamMapNode must be standard-layout for offsetof()");
static_assert(std::is_trivially_copyable_v<FamMapNode>, "FamMapNode must be trivially copyable for FAM put/get");
static_assert(sizeof(FamMapNode) == 40, "FamMapNode layout changed (FAM on-wire format depends on this)");
static_assert(offsetof(FamMapNode, header) == 0, "FamMapNode::header must be the first member (wire layout)");
static_assert(offsetof(FamMapNode, desc) == 16, "FamMapNode::desc offset mismatch");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
