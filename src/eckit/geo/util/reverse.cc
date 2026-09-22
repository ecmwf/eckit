// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <vector>


namespace eckit::geo::util {


std::vector<double> reverse(const std::vector<double>& v) {
    std::vector<double> flipped(v.size());
    std::reverse_copy(v.begin(), v.end(), flipped.begin());

    return v;
}


}  // namespace eckit::geo::util
