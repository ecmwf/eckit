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
#include <limits>
#include <vector>

#include "grit/types.h"


namespace grit::util {


template <typename T>
bool is_approximately_equal(T x, T y, T eps = std::numeric_limits<T>::epsilon()) {
    auto min = std::min(std::abs(x), std::abs(y));
    return std::abs(min) == 0. ? std::abs(x - y) < eps
                               : std::abs(x - y) / std::max(std::numeric_limits<T>::min(), min) < eps;
};


template <typename T>
pl_type pl_convert(const T& pl) {
    ASSERT(!pl.empty());

    pl_type _pl(pl.size());
    std::transform(pl.begin(), pl.end(), _pl.begin(),
                   [](typename T::value_type p) { return static_cast<pl_type::value_type>(p); });
    return std::move(_pl);
}


template <>
pl_type pl_convert(const pl_type&);


static constexpr double degrees_to_radians = M_PI / 180.;


static constexpr double radians_to_degrees = 180. * M_1_PI;


std::vector<double> arange(double start, double stop, double step);


std::vector<double> linspace(double start, double stop, size_t num, bool endpoint);


std::vector<double> gaussian_latitudes(size_t N, bool increasing);


std::vector<double> monotonic_crop(std::vector<double>&, double min, double max, double eps);


/// @return longitude in degree within range [minimum, minimum + 360[
double normalise_longitude_to_minimum(double lon, double minimum);


/// @return longitude in degree within range ]maximum - 360, maximum]
double normalise_longitude_to_maximum(double lon, double maximum);


const pl_type& reduced_classical_pl(size_t N);


const pl_type& reduced_octahedral_pl(size_t N);


pl_type::value_type regular_pl(size_t N);


}  // namespace grit::util
