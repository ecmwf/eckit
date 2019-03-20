/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/ComparatorT.h"

#include "mir/stats/detail/AngleT.h"
#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/MinMax.h"
#include "mir/stats/detail/PNorms.h"
#include "mir/stats/detail/Scalar.h"


namespace mir {
namespace stats {


static ComparatorBuilder<ComparatorT<detail::AngleT<detail::AngleScale::DEGREE>>> __stats1("angle-degree");
static ComparatorBuilder<ComparatorT<detail::AngleT<detail::AngleScale::RADIAN>>> __stats2("angle-radian");
static ComparatorBuilder<ComparatorT<detail::CentralMomentsT<double>>>            __stats3("central-moments");
static ComparatorBuilder<ComparatorT<detail::MinMax>> __stats4("min-max");
static ComparatorBuilder<ComparatorT<detail::PNorms>> __stats5("p-norms");
static ComparatorBuilder<ComparatorT<detail::Scalar>> __stats6("scalar");



}  // namespace stats
}  // namespace mir

