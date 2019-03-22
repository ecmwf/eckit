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

    std::string execute(const data::MIRField& field1, const data::MIRField& field2);

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

    void print(std::ostream& out) const;

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
