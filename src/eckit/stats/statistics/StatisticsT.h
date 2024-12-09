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

#include "eckit/exception/Exceptions.h"
#include "eckit/stats/Field.h"
#include "eckit/stats/Statistics.h"
#include "eckit/stats/detail/Counter.h"


namespace eckit::stats::statistics {


/// Generic statistics on a Field
template <typename STATS>
class StatisticsT : public Statistics, detail::Counter, STATS {
public:
    // -- Constructors

    StatisticsT(const Parametrisation& parametrisation) : Statistics(parametrisation), Counter(parametrisation) {}

    // -- Overridden methods

    void execute(const Field& field) override {
        Counter::reset(field);
        STATS::reset();

        for (auto& value : field.values()) {
            if (count(value)) {
                STATS::operator()(value);
            }
        }
    }

private:
    // -- Overridden methods

    void print(std::ostream& out) const override {
        out << "Statistics[";
        Counter::print(out);
        out << ",";
        STATS::print(out);
        out << "]";
    }
};


}  // namespace eckit::stats::statistics
