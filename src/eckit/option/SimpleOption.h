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


#ifndef eckit_option_SimpleOption_H
#define eckit_option_SimpleOption_H

#include <iosfwd>

#include "eckit/option/Option.h"


namespace eckit {
namespace option {


template <class T>
class SimpleOption : public Option {
public:
    SimpleOption(const std::string& name, const std::string& description);

    virtual ~SimpleOption() override;

protected:
    virtual void print(std::ostream&) const override;

private:
    virtual void set(Configured&) const override;
    virtual void set(const std::string& value, Configured&) const override;
    virtual void copy(const Configuration& from, Configured& to) const override;
};


}  // namespace option
}  // namespace eckit

#include "eckit/option/SimpleOption.cc"

#endif
