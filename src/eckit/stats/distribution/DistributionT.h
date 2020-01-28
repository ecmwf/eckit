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


#ifndef mir_stats_distribution_DistributionT_h
#define mir_stats_distribution_DistributionT_h

#include "mir/stats/Distribution.h"


namespace mir {
namespace param {
class MIRParametrisation;
}
}  // namespace mir


namespace mir {
namespace stats {
namespace distribution {


template <typename DISTRIBUTION >
class DistributionT : public Distribution {
public:
    // -- Exceptions
    // None

    // -- Contructors

    DistributionT(const param::MIRParametrisation&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    mutable DISTRIBUTION distribution_;

    // -- Methods

    typename DISTRIBUTION::param_type param(const param::MIRParametrisation&) const;
    std::string to_string(typename DISTRIBUTION::param_type&) const;

    // -- Overridden methods

    // From Distribution
    void reset();
    double operator()() const;
    void print(std::ostream&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace distribution
}  // namespace stats
}  // namespace mir


#endif
