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


#include "mir/stats/detail/AngleT.h"


namespace mir {
namespace stats {
namespace detail {


/// Angle statistics in degrees [0, 360]
/// (range ] from std::arg(std::complex))
template <>
AngleT<double, AngleScale::DEGREE, AngleSpace::ASYMMETRIC>::AngleT() :
    rescale_(M_1_PI * 180.),
    descale_(M_PI / 180.),
    globe_(360),
    min_(0) {}


/// Angle statistics in degrees [-180,180]
/// (range ] from std::arg(std::complex))
template <>
AngleT<double, AngleScale::DEGREE, AngleSpace::SYMMETRIC>::AngleT() :
    rescale_(M_1_PI * 180.),
    descale_(M_PI / 180.),
    globe_(360),
    min_(-180) {}


/// Angle statistics in radians [0, 2π]
/// (range ] from std::arg(std::complex))
template <>
AngleT<double, AngleScale::RADIAN, AngleSpace::ASYMMETRIC>::AngleT() :
    rescale_(1.),
    descale_(1.),
    globe_(2. * M_PI),
    min_(0) {}


/// Angle statistics in radians [-π,π]
/// (range ] from std::arg(std::complex))
template <>
AngleT<double, AngleScale::RADIAN, AngleSpace::SYMMETRIC>::AngleT() :
    rescale_(1.),
    descale_(1.),
    globe_(2. * M_PI),
    min_(-M_PI) {}


}  // namespace detail
}  // namespace stats
}  // namespace mir
