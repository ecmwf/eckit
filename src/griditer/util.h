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

#include <cstddef>
#include <vector>


namespace grit::util {


std::vector<double> arange(double start, double stop, double step);


std::vector<double> linspace(double start, double stop, size_t num, bool endpoint);


std::vector<double> gaussian_latitudes(size_t N, bool increasing);


}  // namespace grit::util
