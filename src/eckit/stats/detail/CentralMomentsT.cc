/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/detail/CentralMomentsT.h"

#include <complex>
#include <limits>


namespace mir {
namespace stats {
namespace detail {


template<> std::complex<double> CentralMomentsT<std::complex<double>>::mean() const {
    using limits = std::numeric_limits<double>;
    static std::complex<double> NaN {limits::quiet_NaN(), limits::quiet_NaN()};
    return count_ < 1 ? NaN : M1_;
}


template<> std::complex<float> CentralMomentsT<std::complex<float>>::mean() const {
    using limits = std::numeric_limits<float>;
    static std::complex<float> NaN {limits::quiet_NaN(), limits::quiet_NaN()};
    return count_ < 1 ? NaN : M1_;
}


}  // namespace detail
}  // namespace stats
}  // namespace mir
