/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015


#ifndef Separator_H
#define Separator_H

#include <iosfwd>

#include "eckit/option/Option.h"

namespace eckit::option {

class Separator : public Option {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Separator(const std::string& description);

    // -- Destructor

    ~Separator() override;  // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    size_t set(Configured& parameter, size_t values, args_t::const_iterator begin,
               args_t::const_iterator end) const override;

    void setDefault(Configured&) const override;


    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    // -- Methods

    void print(std::ostream&) const override;  // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    Separator(const Separator&);
    Separator& operator=(const Separator&);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    bool active() const override;
    void copy(const Configuration& from, Configured& to) const override;


    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
};

}  // namespace eckit::option

#endif
