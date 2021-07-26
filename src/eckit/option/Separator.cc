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


#include "eckit/option/Separator.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Translator.h"

#include <iostream>

namespace eckit {

namespace option {


Separator::Separator(const std::string& description) :
    Option("", description) {}


Separator::~Separator() {}


void Separator::set(const std::string& value, Configured& parametrisation) const {
    NOTIMP;
}

void Separator::copy(const Configuration& from, Configured& to) const {
    ;
}

bool Separator::active() const {
    return false;
}

void Separator::print(std::ostream& out) const {
    out << std::endl
        << description_ << ":" << std::endl;
}

}  // namespace option

}  // namespace eckit
