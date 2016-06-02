/*
 * (C) Copyright 1996-2016 ECMWF.
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


namespace eckit {

namespace option {

class Separator : public Option {
  public:

// -- Exceptions
    // None

// -- Contructors

    Separator(const std::string& description);

// -- Destructor

    virtual ~Separator(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods
    // None




// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  protected:

// -- Members

// -- Methods

    virtual void print(std::ostream&) const; // Change to virtual if base class

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

    using Option::set;
    virtual void set(const std::string& value, Configured&) const;
    virtual bool active() const;


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends


};


}

} // namespace eckit

#endif

