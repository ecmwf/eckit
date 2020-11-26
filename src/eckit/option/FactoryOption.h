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


#ifndef eckit_option_FactoryOption_H
#define eckit_option_FactoryOption_H

#include <iosfwd>

#include "eckit/option/Option.h"

namespace eckit {
namespace option {


/// @note Factory option essentially provides a std::string option, whose acceptable values are listed on the
///       command line (typename T may be any class that implements the list() method). It does no checks on
///       the validity of input received, and just returns the appropriate string


template <class T>
class FactoryOption : public Option {
public:
    FactoryOption(const std::string& name, const std::string& description);

    virtual ~FactoryOption() override;  // Change to virtual if base class

protected:
    virtual void print(std::ostream&) const override;  // Change to virtual if base class

private:
    using Option::set;

    virtual void set(const std::string& value, Configured&) const override;
    virtual void copy(const Configuration& from, Configured& to) const override;
};


}  // namespace option
}  // namespace eckit

#include "eckit/option/FactoryOption.cc"

#endif
