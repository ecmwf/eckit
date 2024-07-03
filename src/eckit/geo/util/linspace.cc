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


namespace eckit::geo::util {


std::vector<double> linspace(double start, double stop, size_t num, bool endpoint) {
    if (num == 0) {
        return {};
    }

    const auto step = num > 1 ? (stop - start) / static_cast<double>(endpoint ? (num - 1) : num) : 0;

    std::vector<double> l(num);
    std::generate_n(l.begin(), num,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return l;
}


}  // namespace eckit::geo::util
