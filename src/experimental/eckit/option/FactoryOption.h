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


#ifndef eckit_option_FactoryOption_H
#define eckit_option_FactoryOption_H

#include <iosfwd>

#include "eckit/option/Option.h"

namespace eckit {
namespace option {


template<class T>
class FactoryOption : public Option {
  public:

// -- Exceptions
    // None

// -- Contructors

    FactoryOption(const std::string& name, const std::string& description);

// -- Destructor

    virtual ~FactoryOption(); // Change to virtual if base class

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

    FactoryOption(const FactoryOption&);
    FactoryOption& operator=(const FactoryOption&);

// -- Members
    // None

// -- Methods
    // None

// -- Overridden methods

    virtual void set(const std::string& value, LocalConfiguration&) const;


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

};


} // namespace option
} // namespace eckit

#include "eckit/option/FactoryOption.cc"

#endif

