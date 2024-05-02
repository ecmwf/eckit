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


#include <algorithm>
#include <vector>

#include "eckit/types/FloatCompare.h"


namespace eckit::geo::util {


std::vector<double> arange(double start, double stop, double step) {
    if (types::is_approximately_equal(step, 0.) || types::is_approximately_equal(start, stop)
        || (stop - start) * step < 0.) {
        std::vector<double> l(1, start);
        return l;
    }

    const auto num = static_cast<size_t>((stop - start) / step) + 1;

    std::vector<double> l(num);
    std::generate_n(l.begin(), num,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return l;
}


}  // namespace eckit::geo::util
