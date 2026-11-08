// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/util.h"

#include <cctype>


namespace eckit::geo::util {


template <>
pl_type pl_convert(const pl_type& pl) {
    return pl;
}


std::string to_lower(const std::string& str) {
    std::string lower(str);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return lower;
}


std::string to_upper(const std::string& str) {
    std::string upper(str);
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return upper;
}


}  // namespace eckit::geo::util
