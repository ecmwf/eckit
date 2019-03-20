/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_ComparatorT_h
#define mir_stats_ComparatorT_h

#include <cmath>
#include <ostream>

#include "eckit/exception/Exceptions.h"

#include "mir/data/MIRField.h"
#include "mir/stats/Comparator.h"
#include "mir/stats/detail/CounterBinary.h"


namespace mir {
namespace stats {


/// Generic comparison on two MIRFields
template<typename STATS>
class ComparatorT : public Comparator, public STATS {
public:

    // -- Exceptions
    // None

    // -- Constructors

    ComparatorT(const param::MIRParametrisation& parametrisation1, const param::MIRParametrisation& parametrisation2) :
        Comparator(parametrisation1, parametrisation2),
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

    void execute(const data::MIRField& field1, const data::MIRField& field2) {
        detail::CounterBinary counter(field1, field2);
        STATS::reset();

        ASSERT(field1.dimensions() == 1);
        ASSERT(field2.dimensions() == 1);

        auto& values1 = field1.values(0);
        auto& values2 = field2.values(0);
        ASSERT(values1.size() == values2.size());

        for (size_t i = 0; i < values1.size(); ++i) {
            if (counter(values1[i], values2[i])) {
                STATS::operator()(std::abs(values1[i] - values2[i]));
            }
        }

        if (counter.missing1()) {
            throw eckit::BadValue("Different missing values");
        }

        count_   = counter.count();
        missing_ = counter.missing2();
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
        out << "Comparator[count" << count_ << ",missing"  << missing_ << ",";
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
