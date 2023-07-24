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
#include <utility>
#include <vector>


namespace eckit::geometry {


using pl_type = std::vector<long>;


namespace util {


constexpr double degree_to_radian = M_PI / 180.;
constexpr double radian_to_degree = M_1_PI * 180.;


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


std::vector<double> arange(double start, double stop, double step);


std::vector<double> gaussian_latitudes(size_t N, bool increasing);


std::vector<double> linspace(double start, double stop, size_t num, bool endpoint);


std::pair<std::vector<double>::const_iterator, std::vector<double>::const_iterator> monotonic_crop(
    const std::vector<double>&, double min, double max, double eps);


const pl_type& reduced_classical_pl(size_t N);


const pl_type& reduced_octahedral_pl(size_t N);


pl_type::value_type regular_pl(size_t N);


}  // namespace util


}  // namespace eckit::geometry