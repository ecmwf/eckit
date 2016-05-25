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


#ifndef SimpleOption_H
#define SimpleOption_H

#include <iosfwd>

#include "eckit/option/Option.h"


namespace eckit {

namespace option {


template<class T>
class SimpleOption : public Option {
  public:

// -- Exceptions
    // None

// -- Contructors

    SimpleOption(const std::string& name, const std::string& description);

// -- Destructor

    virtual ~SimpleOption(); // Change to virtual if base class

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

    SimpleOption(const SimpleOption&);
    SimpleOption& operator=(const SimpleOption&);

// -- Members
    // None

// -- Methods
    // None

// -- Overridden methods

    virtual void set(LocalConfiguration&) const;
    virtual void set(const std::string& value, LocalConfiguration&) const;


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends


};


} // namespace option

} // namespace eckit

#include "eckit/option/SimpleOption.cc"

#endif

