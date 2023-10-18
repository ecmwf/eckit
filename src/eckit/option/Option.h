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
/// @author Pedro Maciel
/// @date Apr 2015


#pragma once

#include <iosfwd>
#include <string>

#include "eckit/memory/NonCopyable.h"

#include "eckit/exception/Exceptions.h"  // FIXME: remove this, dependants have wrong includes


namespace eckit {
class Configuration;
class Configured;
}  // namespace eckit


namespace eckit::option {


template <class T>
struct Title {
    const char* operator()() const;
};


class Option : private NonCopyable {
public:
    // -- Contructors

    Option(const std::string& name, const std::string& description);

    // -- Destructor

    virtual ~Option() = default;

    // -- Methods

    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }

    virtual Option* defaultValue(const std::string&);
    virtual bool active() const;
    virtual void set(Configured&) const;
    virtual void setDefault(Configured&) const;

    virtual void set(const std::string& value, Configured&) const      = 0;
    virtual void copy(const Configuration& from, Configured& to) const = 0;

protected:
    // -- Methods

    virtual void print(std::ostream&) const = 0;

private:
    // -- Members

    std::string name_;
    std::string description_;
    std::string default_;
    bool hasDefault_;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Option& p) {
        p.print(s);
        return s;
    }
};


}  // namespace eckit::option
