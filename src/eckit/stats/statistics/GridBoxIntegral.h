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


#ifndef mir_stats_statistics_GridBoxIntegral_h
#define mir_stats_statistics_GridBoxIntegral_h

#include "mir/stats/Statistics.h"

#include "mir/stats/detail/Counter.h"


namespace mir {
namespace stats {
namespace statistics {


class GridBoxIntegral : public Statistics, detail::Counter {
public:

    // -- Exceptions
    // None

    // -- Constructors

    GridBoxIntegral(const param::MIRParametrisation&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void reset();

    double integral() const {
        return integral_;
    }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    double integral_;

    // -- Methods
    // None

    // -- Overridden methods

    void execute(const data::MIRField&);
    void print(std::ostream&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace statistics
}  // namespace stats
}  // namespace mir


#endif
