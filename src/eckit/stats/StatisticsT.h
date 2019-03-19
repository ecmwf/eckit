/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_StatisticsT_h
#define mir_stats_StatisticsT_h

#include <cmath>
#include <ostream>

#include "eckit/exception/Exceptions.h"

#include "mir/data/MIRField.h"
#include "mir/stats/Statistics.h"


namespace mir {
namespace stats {


/// Generic statistics on a MIRField
template<typename STATS>
class StatisticsT : public Statistics, public STATS {
public:

    // -- Exceptions
    // None

    // -- Constructors

    StatisticsT(const param::MIRParametrisation& parametrisation) :
        Statistics(parametrisation),
        count_(0),
        missing_(0) {
    }

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    size_t count() const {
        return count_;
    }

    size_t missing() const{
        return missing_;
    }

    // -- Overridden methods

    void execute(const data::MIRField& field) {
        CounterUnary counter(field);
        STATS::reset();

        ASSERT(field.dimensions() == 1);
        for (auto& value : field.values(0)) {
            if (!counter.missingValue(value)) {
                STATS::operator()(value);
            }
        }

        count_   = counter.count();
        missing_ = counter.missing();
    }

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    size_t count_;
    size_t missing_;

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream& out) const {
        out << "Statistics[count" << count_ << ",missing"  << missing_ << ",";
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


}  // namespace stats
}  // namespace mir


#endif
