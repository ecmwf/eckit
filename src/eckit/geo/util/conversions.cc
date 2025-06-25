/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <cstddef>
#include <limits>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::util {


template <typename A, typename B>
B convert_A_to_B(A a) {
    constexpr auto max = std::numeric_limits<B>::max();

    const auto b = static_cast<B>(a);
    ASSERT(b < max);
    return b;
}


size_t convert_long_to_size_t(long l) {
    ASSERT(0 <= l);
    return convert_A_to_B<long, size_t>(l);
}


}  // namespace eckit::geo::util
