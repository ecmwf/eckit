/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#ifndef mir_stats_ScalarCentralMoments_h
#define mir_stats_ScalarCentralMoments_h

#include "mir/stats/Statistics.h"
#include "mir/stats/detail/ScalarCentralMomentsFn.h"


namespace mir {
namespace action {
class MIRField;
}
}

namespace mir {
namespace stats {


/**
 * @brief Calculate statistics on a MIRField
 */
class ScalarCentralMoments : public Statistics {
public:

    // -- Exceptions
    // None

    // -- Constructors

    ScalarCentralMoments(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~ScalarCentralMoments() {}

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    /// Online statistics update
    void operator+=(const ScalarCentralMoments&);

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

    /// Calculate statistics
    Results calculate(const data::MIRField&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    mutable detail::ScalarCentralMomentsFn<double> stats_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace stats
}  // namespace mir


#endif

