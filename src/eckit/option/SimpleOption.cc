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

#include <iostream>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/utils/Translator.h"

namespace eckit {

namespace option {

template <class T>
struct Title {
    const char* operator()() const;
};

template <>
const char* Title<size_t>::operator()() const;

template <>
const char* Title<long>::operator()() const;

template <>
const char* Title<double>::operator()() const;

template <>
const char* Title<bool>::operator()() const;

template <>
const char* Title<std::string>::operator()() const;

template <>
const char* Title<eckit::PathName>::operator()() const;

template <class T>
SimpleOption<T>::SimpleOption(const std::string& name, const std::string& description) :
    Option(name, description) {}

template <class T>
SimpleOption<T>::~SimpleOption() {}

template <class T>
void SimpleOption<T>::set(const std::string& value, Configured& parametrisation) const {
    T v = eckit::Translator<std::string, T>()(value);
    parametrisation.set(name_, v);
}

template <class T>
void SimpleOption<T>::set(Configured& parametrisation) const {
    Option::set(parametrisation);
}


template <class T>
void SimpleOption<T>::copy(const Configuration& from, Configured& to) const {
    T v;
    if (from.get(name_, v)) {
        to.set(name_, v);
    }
}

template <>
void SimpleOption<eckit::PathName>::set(const std::string& value, Configured& parametrisation) const;

template <>
void SimpleOption<eckit::PathName>::copy(const Configuration& from, Configured& to) const;

template <>
void SimpleOption<bool>::print(std::ostream& out) const;

template <class T>
void SimpleOption<T>::print(std::ostream& out) const {
    out << "   --" << name_ << "=" << Title<T>()() << " (" << description_ << ")";
}

template <>
void SimpleOption<bool>::set(Configured& parametrisation) const;

}  // namespace option

}  // namespace eckit
