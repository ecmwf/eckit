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


namespace eckit::stats::statistics {


/**
 * @brief Calculate spectral statistics on a Field
 */
class Spectral : public Statistics {
public:
    // -- Constructors

    Spectral(const Parametrisation&);

    // -- Destructor

    ~Spectral() override;

    // -- Methods

    void reset();
    double mean() const;
    double variance() const;
    double standardDeviation() const;
    double enorm() const;

    // -- Overridden methods

    void execute(const Field&) override;

private:
    // -- Members

    double mean_;
    double variance_;
    double stddev_;
    double enorm_;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::stats::statistics
