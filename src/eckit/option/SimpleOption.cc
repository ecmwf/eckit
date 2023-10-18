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


#include "eckit/option/SimpleOption.h"


namespace eckit::option {


template <>
void SimpleOption<eckit::PathName>::set(const std::string& value, Configured& to) const {
    to.set(name(), value);
}


template <>
void SimpleOption<eckit::PathName>::copy(const Configuration& from, Configured& to) const {
    std::string v;
    if (from.get(name(), v)) {
        to.set(name(), v);
    }
}


template <>
void SimpleOption<bool>::print(std::ostream& out) const {
    out << "   --" << name() << " (" << description() << ")";
}


template <>
void SimpleOption<bool>::set(Configured& to) const {
    to.set(name(), true);
}


}  // namespace eckit::option
