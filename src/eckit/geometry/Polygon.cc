#include <iostream>

#include "eckit/geometry/Polygon.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//----------------------------------------------------------------------------------------------------------------------

bool Polygon::congruent(const Polygon& p) const {
    if (vertices.size() != p.vertices.size())
        return false;
    if (vertices.empty())
        return true;
    int offset = -1;

    for (int i = 0; i < vertices.size(); i++) {
        if (vertices.at(i) == p.vertices.at(0)) {
            offset = i;
            break;
        }
    }
    if (offset == -1)
        return false;
    for (int i = 1; i < vertices.size(); i++) {
        if (vertices.at((i + offset) % vertices.size()) != p.vertices.at(i)) {
            return false;
        }
    }
    return true;
}

void Polygon::print(std::ostream& s) const {
    if (vertices.empty()) {
        s << "[]";
        return;
    }

    char z = '[';
    for (auto& v : vertices) {
        s << z << v;
        z = ',';
    }
    s << ']';
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit
