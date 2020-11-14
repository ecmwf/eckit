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


#ifndef Option_H
#define Option_H

#include <iosfwd>
#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class Configuration;
class Configured;

namespace option {


class Option : private eckit::NonCopyable {
public:  // methods
    Option(const std::string& name, const std::string& description);

    virtual ~Option();  // Change to virtual if base class

    virtual Option* defaultValue(const std::string&);

    const std::string& name() const;

    virtual bool active() const;

    virtual void set(Configured&) const;
    virtual void set(const std::string& value, Configured&) const      = 0;
    virtual void copy(const Configuration& from, Configured& to) const = 0;
    virtual void setDefault(Configured&) const;

protected:  // members
    std::string name_;
    std::string description_;

    bool hasDefault_;
    std::string default_;

    virtual void print(std::ostream&) const = 0;  // Change to virtual if base class

private:
    friend std::ostream& operator<<(std::ostream& s, const Option& p) {
        p.print(s);
        return s;
    }
};

}  // namespace option
}  // namespace eckit

#endif
