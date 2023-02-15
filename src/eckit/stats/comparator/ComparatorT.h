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

#include <iosfwd>
#include <limits>
#include <memory>
#include <string>

#include "mir/param/SameParametrisation.h"
#include "mir/stats/Comparator.h"
#include "mir/stats/detail/CounterBinary.h"


namespace mir::data {
class MIRField;
}  // namespace mir::data


namespace mir::stats::comparator {


/// Generic comparison on two MIRFields
template <typename STATS>
class ComparatorT : public Comparator, detail::CounterBinary, STATS {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ComparatorT(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) :
        Comparator(param1, param2),
        CounterBinary(param1, param2),
        ignoreAboveLatitude_(std::numeric_limits<double>::quiet_NaN()),
        ignoreBelowLatitude_(std::numeric_limits<double>::quiet_NaN()) {

        std::unique_ptr<param::MIRParametrisation> param(new param::SameParametrisation(param1, param2, false));
        param->get("ignore-above-latitude", ignoreAboveLatitude_);
        param->get("ignore-below-latitude", ignoreBelowLatitude_);
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

    std::string execute(const data::MIRField& field1, const data::MIRField& field2) override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    double ignoreAboveLatitude_;
    double ignoreBelowLatitude_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Comparator
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::stats::comparator
