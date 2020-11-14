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


#ifndef VectorOption_H
#define VectorOption_H

#include <iosfwd>

#include "eckit/option/Option.h"
#include "eckit/option/SimpleOption.h"


namespace eckit {

namespace option {


template <class T>
class VectorOption : public Option {
public:
    // -- Exceptions
    // None

    // -- Contructors

    VectorOption(const std::string& name, const std::string& description, size_t size, const char* separator = "/");

    // -- Destructor

    virtual ~VectorOption() override;  // Change to virtual if base class

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

    virtual void print(std::ostream&) const override;  // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // No copy allowed

    VectorOption(const VectorOption&);
    VectorOption& operator=(const VectorOption&);

    // -- Members

    size_t size_;
    const char* separator_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual void set(Configured&) const override;
    virtual void set(const std::string& value, Configured&) const override;
    virtual void copy(const Configuration& from, Configured& to) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
};


}  // namespace option

}  // namespace eckit

#include "eckit/option/VectorOption.cc"

#endif
