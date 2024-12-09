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

#include "eckit/stats/Comparator.h"


namespace eckit::stats::comparator {


/**
 * @brief Calculate spectral statistics on a Field
 */
class Spectral : public Comparator {
public:
    // -- Constructors

    Spectral(const Parametrisation&, const Parametrisation&);

    // -- Destructor

    ~Spectral() override;

    // -- Methods

    void reset();
    double meanDiff() const;
    double enormDiff() const;

private:
    // -- Members

    double meanDiffMax_;
    double enormDiffMax_;

    double meanDiff_;
    double enormDiff_;
    std::string stats_;

    // -- Overridden methods

    std::string execute(const Field&, const Field&) override;
    void print(std::ostream&) const override;
};


}  // namespace eckit::stats::comparator
