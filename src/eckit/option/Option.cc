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


#include "eckit/option/Option.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::option {

Option::Option(const std::string& name, const std::string& description) :
    name_(name), description_(description) {}

std::ostream& operator<<(std::ostream& s, const Option& p)
{
    p.print(s);
    return s;
}

}  // namespace eckit::option
