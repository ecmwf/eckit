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


#ifndef Option_H
#define Option_H

#include <iosfwd>
#include <string>


namespace eckit {

class LocalConfiguration;

namespace option {


class Option {
  public:

// -- Exceptions
    // None

// -- Contructors

    Option(const std::string& name, const std::string& description);

// -- Destructor

    virtual ~Option(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods
    // None

    const std::string& name() const;

    virtual bool active() const;

    virtual void set(LocalConfiguration&) const;
    virtual void set(const std::string& value, LocalConfiguration&) const = 0;


// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  protected:

// -- Members

    std::string name_;
    std::string description_;

// -- Methods

    virtual void print(std::ostream&) const = 0; // Change to virtual if base class

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

    Option(const Option&);
    Option& operator=(const Option&);

// -- Members
    // None

// -- Methods
    // None

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Option& p) {
        p.print(s);
        return s;
    }

};

} // namespace option

} // namespace eckit
#endif

