// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
