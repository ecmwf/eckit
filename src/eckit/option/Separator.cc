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


#include "eckit/option/Separator.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::option {


Separator::Separator(const std::string& description) : Option("", description) {}


void Separator::set(const std::string& value, Configured& parametrisation) const {
    NOTIMP;
}


void Separator::copy(const Configuration& from, Configured& to) const {
    // FIXME this function should not be overriding to do nothing
}


bool Separator::active() const {
    return false;
}


void Separator::print(std::ostream& out) const {
    out << "\n" << description() << ":" << std::endl;
}


}  // namespace eckit::option
