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


#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>


namespace eckit::geo {


using difference_type = std::make_signed_t<size_t>;
using pl_type         = std::vector<long>;


namespace util {


constexpr double DEGREE_TO_RADIAN = M_PI / 180.;
constexpr double RADIAN_TO_DEGREE = M_1_PI * 180.;


template <typename T>
pl_type pl_convert(const T& pl) {
    ASSERT(!pl.empty());

    pl_type _pl(pl.size());
    std::transform(pl.begin(), pl.end(), _pl.begin(),
                   [](typename T::value_type p) { return static_cast<pl_type::value_type>(p); });
    return _pl;
}


template <>
pl_type pl_convert(const pl_type&);


size_t convert_long_to_size_t(long);


std::vector<double> arange(double start, double stop, double step);


const std::vector<double>& gaussian_latitudes(size_t N, bool increasing);


std::vector<double> linspace(double start, double stop, size_t num, bool endpoint);


std::pair<difference_type, difference_type> monotonic_crop(const std::vector<double>&, double min, double max,
                                                           double eps);


bool reduced_classical_pl_known(size_t N);


const pl_type& reduced_classical_pl(size_t N);


const pl_type& reduced_octahedral_pl(size_t N);


}  // namespace util


}  // namespace eckit::geo
