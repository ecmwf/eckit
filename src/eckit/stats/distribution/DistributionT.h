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

#include "eckit/stats/Distribution.h"


namespace eckit::param {
class MIRParametrisation;
}  // namespace eckit::param


namespace eckit::stats::distribution {


template <typename DISTRIBUTION>
class DistributionT : public Distribution {
public:
    // -- Constructors

    DistributionT(const param::MIRParametrisation&);

protected:
private:
    // -- Members

    mutable DISTRIBUTION distribution_;

    // -- Methods

    typename DISTRIBUTION::param_type param(const param::MIRParametrisation&) const;
    std::string to_string(typename DISTRIBUTION::param_type&) const;

    // -- Overridden methods

    // From Distribution
    void reset() override;
    double operator()() const override;
    void print(std::ostream&) const override;
};


}  // namespace eckit::stats::distribution
