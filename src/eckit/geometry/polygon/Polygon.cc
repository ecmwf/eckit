/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/geometry/polygon/Polygon.h"

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
