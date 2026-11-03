// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>


namespace eckit::geo {


using difference_type = std::make_signed_t<size_t>;
using pl_type         = std::vector<long>;  // sync with eckit::spec::Custom number_type


namespace util {


constexpr double DEGREE_TO_RADIAN = M_PI / 180.;
constexpr double RADIAN_TO_DEGREE = M_1_PI * 180.;


/// pl (number of points per latitude) of any integer type, as pl_type
template <typename T>
pl_type pl_convert(const T& pl) {
    ASSERT(!pl.empty());

    pl_type _pl(pl.size());
    std::transform(pl.begin(), pl.end(), _pl.begin(),
                   [](typename T::value_type p) { return static_cast<pl_type::value_type>(p); });
    return _pl;
}


/// (no conversion)
template <>
pl_type pl_convert(const pl_type&);


/// Values from start to stop (inclusive, if reached) by step; only start if step is 0 or away from stop
std::vector<double> arange(double start, double stop, double step);


/// Gaussian latitudes [degree] of Gaussian number N (2 N values), increasing or decreasing; cached
const std::vector<double>& gaussian_latitudes(size_t N, bool increasing);


/// Gaussian quadrature weights of Gaussian number N (2 N values, symmetric); cached
const std::vector<double>& gaussian_quadrature_weights(size_t N);


/// num evenly spaced values from start to stop (inclusive); cached
const std::vector<double>& linspace(double start, double stop, size_t num);


/// Index range [first, last) of the values in [min, max], of monotonic (increasing or decreasing) values
std::pair<difference_type, difference_type> monotonic_crop(const std::vector<double>&, double min, double max);


/// Values in reverse order
std::vector<double> reverse(const std::vector<double>&);


/// If the reduced classical Gaussian pl of Gaussian number N is tabulated
bool reduced_classical_pl_known(size_t N);


/// Reduced classical Gaussian pl of Gaussian number N (2 N values, tabulated); cached
const pl_type& reduced_classical_pl(size_t N);


/// Reduced octahedral Gaussian pl of Gaussian number N (2 N values, 20, 24, ... to the equator); cached
const pl_type& reduced_octahedral_pl(size_t N);


}  // namespace util


}  // namespace eckit::geo
