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


#ifndef mir_stats_statistics_MethodT_h
#define mir_stats_statistics_MethodT_h

#include <ostream>
#include <vector>

#include "mir/stats/Method.h"
#include "mir/stats/detail/Counter.h"


namespace mir {
namespace data {
class MIRField;
}
}  // namespace mir


namespace mir {
namespace stats {
namespace method {


/// Generic statistics on a MIRField
template <typename STATS>
class MethodT : public Method, public detail::Counter, protected std::vector<STATS> {
private:
    // -- Exceptions

    using vector_t = std::vector<STATS>;

public:
    // -- Exceptions
    // None

    // -- Constructors

    MethodT(const param::MIRParametrisation& parametrisation) : Method(parametrisation), Counter(parametrisation) {}

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void resize(size_t);
    void execute(const data::MIRField&);
    void mean(data::MIRField&) const;
    void variance(data::MIRField&) const;
    void stddev(data::MIRField&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods

    using vector_t::empty;
    using vector_t::size;

    // -- Overridden methods

    void print(std::ostream& out) const {
        out << "Method[";
        Counter::print(out);
        out << "]";
    }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace method
}  // namespace stats
}  // namespace mir


#endif
