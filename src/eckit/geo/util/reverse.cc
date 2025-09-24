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


std::vector<double> reverse(const std::vector<double>& v) {
    std::vector<double> flipped(v.size());
    std::reverse_copy(v.begin(), v.end(), flipped.begin());

    return v;
}


}  // namespace eckit::geo::util
