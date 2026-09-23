// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geometry/polygon/Polygon.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry::polygon {

//----------------------------------------------------------------------------------------------------------------------

bool Polygon::congruent(const Polygon& p) const {
    if (empty()) {
        return true;
    }

    if (size() != p.size()) {
        return false;
    }

    int offset = -1;
    for (int i = 0; i < size(); i++) {
        if (at(i) == p.at(0)) {
            offset = i;
            break;
        }
    }

    if (offset == -1) {
        return false;
    }

    for (int i = 1; i < size(); i++) {
        if (at((i + offset) % size()) != p.at(i)) {
            return false;
        }
    }
    return true;
}

void Polygon::print(std::ostream& s) const {
    if (empty()) {
        s << "[]";
        return;
    }

    char z = '[';
    for (const auto& v : *this) {
        s << z << v;
        z = ',';
    }
    s << ']';
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry::polygon
