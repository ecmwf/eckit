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
#include <cstddef>
#include <limits>
#include <vector>


namespace grit::util {


template <typename T>
bool approximately_equal(T x, T y, T eps = std::numeric_limits<T>::epsilon()) {
    auto min = std::min(std::abs(x), std::abs(y));
    return std::abs(min) == 0. ? std::abs(x - y) < eps
                               : std::abs(x - y) / std::max(std::numeric_limits<T>::min(), min) < eps;
};


std::vector<double> arange(double start, double stop, double step);


std::vector<double> linspace(double start, double stop, size_t num, bool endpoint);


std::vector<double> gaussian_latitudes(size_t N, bool increasing);


/// @return longitude in degree within range [minimum, minimum + 360[
double normalise_longitude_to_minimum(double lon, double minimum);


/// @return longitude in degree within range ]maximum - 360, maximum]
double normalise_longitude_to_maximum(double lon, double maximum);


}  // namespace grit::util
