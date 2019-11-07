/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_statistics_StatisticsT_h
#define mir_stats_statistics_StatisticsT_h

#include <ostream>

#include "eckit/exception/Exceptions.h"

#include "mir/data/MIRField.h"
#include "mir/stats/Statistics.h"
#include "mir/stats/detail/Counter.h"


namespace mir {
namespace stats {
namespace statistics {


/// Generic statistics on a MIRField
template<typename STATS>
class StatisticsT : public Statistics, detail::Counter, STATS {
public:

    // -- Exceptions
    // None

    // -- Constructors

    StatisticsT(const param::MIRParametrisation& parametrisation) :
        Statistics(parametrisation),
        Counter(parametrisation) {
    }

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void execute(const data::MIRField& field) {
        Counter::reset(field);
        STATS::reset();

        ASSERT(field.dimensions() == 1);
        for (auto& value : field.values(0)) {
            if (count(value)) {
                STATS::operator()(value);
            }
        }
    }

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream& out) const {
        out << "Statistics[";
        Counter::print(out);
        out << ",";
        STATS::print(out);
        out << "]";
    }

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
