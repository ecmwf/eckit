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

#include "eckit/stats/Statistics.h"

#include "eckit/stats/detail/Counter.h"


namespace eckit::stats::statistics {


class Integral : public Statistics, detail::Counter {
public:
    // -- Constructors

    Integral(const param::MIRParametrisation&);

    // -- Methods

    void reset();

    double integral() const { return integral_; }

private:
    // -- Members

    double integral_;

    // -- Overridden methods

    void execute(const data::MIRField&) override;
    void print(std::ostream&) const override;
};


}  // namespace eckit::stats::statistics
