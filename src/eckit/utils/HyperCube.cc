/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/HyperCube.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static void addLoop(Ordinal d, Ordinal which, Ordinal where, Ordinal count, Ordinal depth, HyperCube& target,
                    const HyperCube::Dimensions& dims, HyperCube::Coordinates& coord, HyperCube::Remapping& remap) {
    if (d == depth)
        remap.push_back(target.index(coord));
    else {

        int k = 0;
        for (size_t i = 0; i < dims[d]; i++, k++) {
            if (which == d && i == where)
                k += count;
            coord[d] = k;
            addLoop(d + 1, which, where, count, depth, target, dims, coord, remap);
        }
    }
}

HyperCube HyperCube::addToDimension(Ordinal which, Ordinal where, Ordinal howMuch, Remapping& remap) const {

    remap.clear();
    remap.reserve(count());

    Dimensions newdims = dimensions_;
    Coordinates coord(dimensions_.size());


    newdims[which] += howMuch;

    HyperCube target(newdims);

    addLoop(0, which, where, howMuch, dimensions_.size(), target, dimensions_, coord, remap);

    return target;
}

void HyperCube::coordinates(Ordinal index, Coordinates& result) const {
    ASSERT(result.size() == dimensions_.size());

    for (int i = dimensions_.size() - 1; i >= 0; i--) {
        result[i] = (index % dimensions_[i]);
        index /= dimensions_[i];
    }

    ASSERT(index == 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
