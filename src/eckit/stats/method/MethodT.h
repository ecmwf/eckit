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
#include <vector>

#include "eckit/stats/Method.h"
#include "eckit/stats/detail/Counter.h"


namespace eckit::data {
class MIRField;
}  // namespace eckit::data


namespace eckit::stats::method {


/// Generic statistics on a MIRField
template <typename STATS>
class MethodT : public Method, public detail::Counter, protected std::vector<STATS> {
private:
    // -- Exceptions

    using vector_t = std::vector<STATS>;

public:
    // -- Constructors

    MethodT(const param::MIRParametrisation& parametrisation) : Method(parametrisation), Counter(parametrisation) {}

    // -- Overridden methods

    void resize(size_t) override;
    void execute(const data::MIRField&) override;
    void mean(data::MIRField&) const override;
    void variance(data::MIRField&) const override;
    void stddev(data::MIRField&) const override;

private:
    // -- Methods

    using vector_t::empty;
    using vector_t::size;

    // -- Overridden methods

    void print(std::ostream& out) const override {
        out << "Method[";
        Counter::print(out);
        out << "]";
    }
};


}  // namespace eckit::stats::method
