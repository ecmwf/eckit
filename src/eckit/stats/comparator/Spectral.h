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

#include "mir/stats/Comparator.h"


namespace mir::stats::comparator {


/**
 * @brief Calculate spectral statistics on a MIRField
 */
class Spectral : public Comparator {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Spectral(const param::MIRParametrisation&, const param::MIRParametrisation&);

    // -- Destructor

    ~Spectral() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void reset();
    double meanDiff() const;
    double enormDiff() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    double meanDiffMax_;
    double enormDiffMax_;

    double meanDiff_;
    double enormDiff_;
    std::string stats_;

    // -- Methods
    // None

    // -- Overridden methods

    std::string execute(const data::MIRField&, const data::MIRField&) override;
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::stats::comparator
