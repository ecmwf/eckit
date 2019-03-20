/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_comparator_ComparatorT_h
#define mir_stats_comparator_ComparatorT_h

#include <cmath>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

#include "mir/data/MIRField.h"
#include "mir/stats/Comparator.h"
#include "mir/stats/detail/CounterBinary.h"


namespace mir {
namespace stats {
namespace comparator {


/// Generic comparison on two MIRFields
template<typename STATS>
class ComparatorT : public Comparator, detail::CounterBinary, STATS {
public:

    // -- Exceptions
    // None

    // -- Constructors

    ComparatorT(const param::MIRParametrisation& parametrisation1, const param::MIRParametrisation& parametrisation2) :
        Comparator(parametrisation1, parametrisation2),
        CounterBinary(parametrisation1, parametrisation2) {
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

    void execute(const data::MIRField& field1, const data::MIRField& field2) {
        CounterBinary::reset(field1, field2);
        STATS::reset();

        ASSERT(field1.dimensions() == 1);
        ASSERT(field2.dimensions() == 1);

        auto& values1 = field1.values(0);
        auto& values2 = field2.values(0);
        ASSERT(values1.size() == values2.size());

        for (size_t i = 0; i < values1.size(); ++i) {
            if (count(values1[i], values2[i])) {
                STATS::operator()(std::abs(values1[i] - values2[i]));
            }
        }

        if (!CounterBinary::check()) {
            eckit::Log::error() << *this << std::endl;
            throw eckit::BadValue("Comparison failed");
        }

        eckit::Log::info() << *this << std::endl;
    }

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members
    //None

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream& out) const {
        out << "Comparator[";
        CounterBinary::print(out);
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


}  // namespace comparator
}  // namespace stats
}  // namespace mir


#endif
