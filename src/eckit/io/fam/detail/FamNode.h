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

#include "eckit/io/fam/FamObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FamNode {
    FamDescriptor next;
    FamDescriptor prev;
    fam::size_t   length {0};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto getNext(const FamObject& object) -> FamDescriptor {
    return object.get<FamDescriptor>(offsetof(FamNode, next));
}

inline auto getPrev(const FamObject& object) -> FamDescriptor {
    return object.get<FamDescriptor>(offsetof(FamNode, prev));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
