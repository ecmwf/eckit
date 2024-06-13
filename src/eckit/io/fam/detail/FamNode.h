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

/// @file   FamNode.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FamNode {
    std::uint8_t  version {1};  // 1 byte
    FamDescriptor next;
    FamDescriptor prev;
    fam::size_t   length {0};

    //------------------------------------------------------------------------------------------------------------------
    // HELPERS (DO NOT add any virtual function here)

    static auto getNext(const FamObject& object) -> FamDescriptor {
        return object.get<FamDescriptor>(offsetof(FamNode, next));
    }

    static auto getNextOffset(const FamObject& object) -> std::uint64_t {
        return object.get<std::uint64_t>(offsetof(FamNode, next.offset));
    }

    static auto getPrev(const FamObject& object) -> FamDescriptor {
        return object.get<FamDescriptor>(offsetof(FamNode, prev));
    }

    static auto getPrevOffset(const FamObject& object) -> std::uint64_t {
        return object.get<std::uint64_t>(offsetof(FamNode, prev.offset));
    }

    static auto getLength(const FamObject& object) -> fam::size_t {
        return object.get<fam::size_t>(offsetof(FamNode, length));
    }

    static void getData(const FamObject& object, Buffer& buffer) {
        if (const auto length = getLength(object); length > 0) {
            buffer.resize(length);
            object.get(buffer.data(), sizeof(FamNode), length);
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
