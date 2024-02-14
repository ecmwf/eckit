
#include <iostream>

#include "eckit/geo/KPoint.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geo {

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

}  // namespace eckit::geo