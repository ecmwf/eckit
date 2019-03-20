/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_comparator_Spectral_h
#define mir_stats_comparator_Spectral_h

#include "mir/stats/Comparator.h"


namespace mir {
namespace stats {
namespace comparator {


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

    virtual ~Spectral();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void reset();
    double meandiff() const;
    double enormdiff() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    double meanDiff_;
    double enormDiff_;
    std::string stats_;

    // -- Methods
    // None

    // -- Overridden methods

    void execute(const data::MIRField&, const data::MIRField&);
    void print(std::ostream&) const;

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

