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


#include "eckit/option/Option.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

namespace option {


Option::Option(const std::string &name, const std::string &description):
    name_(name),
    description_(description) {
}


Option::~Option() {
}

const std::string &Option::name() const {
    return name_;
}


bool Option::active() const {
    return true;
}

void Option::set(LocalConfiguration &) const {
    std::ostringstream os;
    os << "Option::set() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

} // namespace option

} // namespace eckit

