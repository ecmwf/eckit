
#include <iostream>

#include "eckit/geometry/KPoint.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//----------------------------------------------------------------------------------------------------------------------

template <int SIZE>
void KPoint<SIZE>::print(std::ostream& s) const {
    char z = '{';
    for (size_t i = 0; i < SIZE; ++i) {
        s << z << x_[i];
        z = ',';
    }
    s << '}';
}

template void KPoint<2>::print(std::ostream&) const;
template void KPoint<3>::print(std::ostream&) const;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit
