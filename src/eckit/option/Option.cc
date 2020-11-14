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
#include "eckit/option/SimpleOption.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {

namespace option {


Option::Option(const std::string& name, const std::string& description) :
    name_(name), description_(description), hasDefault_(false) {}


Option::~Option() {}

const std::string& Option::name() const {
    return name_;
}


bool Option::active() const {
    return true;
}

void Option::set(Configured&) const {
    std::ostringstream os;
    os << "Option::set() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Option* Option::defaultValue(const std::string& value) {
    hasDefault_ = true;
    default_    = value;
    return this;
}

void Option::setDefault(Configured& configured) const {
    if (hasDefault_) {
        set(default_, configured);
    }
}


template <>
const char* Title<size_t>::operator()() const {
    return "ordinal";
}

template <>
const char* Title<long>::operator()() const {
    return "integer";
}

template <>
const char* Title<double>::operator()() const {
    return "real";
}

template <>
const char* Title<bool>::operator()() const {
    return "0/1";
}

template <>
const char* Title<std::string>::operator()() const {
    return "string";
}

template <>
const char* Title<eckit::PathName>::operator()() const {
    return "path";
}

template <>
void SimpleOption<bool>::set(Configured& parametrisation) const {
    parametrisation.set(name_, true);
}

template <>
void SimpleOption<eckit::PathName>::set(const std::string& value, Configured& parametrisation) const {
    parametrisation.set(name_, value);
}

template <>
void SimpleOption<eckit::PathName>::copy(const Configuration& from, Configured& to) const {
    std::string v;
    if (from.get(name_, v)) {
        to.set(name_, v);
    }
}

template <>
void SimpleOption<bool>::print(std::ostream& out) const {
    out << "   --" << name_ << " (" << description_ << ")";
}


}  // namespace option

}  // namespace eckit
