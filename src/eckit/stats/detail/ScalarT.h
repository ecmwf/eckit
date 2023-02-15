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

#include <ostream>

#include "mir/stats/detail/CentralMomentsT.h"


namespace mir::stats::detail {


/// Scalar statistics (composition)
template <typename T>
struct ScalarT : public CentralMomentsT<T> {
    void print(std::ostream& out) const override {
        using t = CentralMomentsT<T>;
        out << "Scalar["
               "mean="
            << t::mean() << ",stddev=" << t::standardDeviation() << ",skewness=" << t::skewness()
            << ",kurtosis=" << t::kurtosis() << "]";
    }
};


}  // namespace mir::stats::detail
