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

#pragma once

#include <iostream>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/Title.h"
#include "eckit/utils/Translator.h"

namespace eckit::option {

template <class T>
SimpleOption<T>::SimpleOption(const std::string& name, const std::string& description) : base_t(name, description) {}

template <class T>
SimpleOption<T>::SimpleOption(const std::string& name, const std::string& description, const T& default_value) :
    base_t(name, description, default_value) {}

template <>
inline size_t SimpleOption<bool>::set(Configured& parametrisation, [[maybe_unused]] size_t values,
                                      args_t::const_iterator begin, [[maybe_unused]] args_t::const_iterator end) const {
    // When handling bool, we might not have a value in the range [begin, end), thus the need for this specialization
    if (values > 0) {
        // Take first value in range [begin, end)
        bool value = translate(*begin);
        set_value(value, parametrisation);
        return 1;
    }
    // Nothing to take from range [begin, end)
    set_value(true, parametrisation);
    return 0;
}

template <class T>
size_t SimpleOption<T>::set(Configured& parametrisation, [[maybe_unused]] size_t values, args_t::const_iterator begin,
                            args_t::const_iterator end) const {
    if (begin == end) {
        throw UserError("No option value found for SimpleOption, where 1 was expected");
    }
    // Take first value in range [begin, end)
    T value = translate(*begin);
    set_value(value, parametrisation);
    return 1;
}

template <class T>
void SimpleOption<T>::set_value(const T& value, Configured& parametrisation) const {
    parametrisation.set(this->name(), value);
}

template <class T>
T SimpleOption<T>::translate(const std::string& value) const {
    T v = Translator<std::string, T>()(value);
    return v;
}

template <>
inline void SimpleOption<PathName>::copy(const Configuration& from, Configured& to) const {
    std::string v;
    if (from.get(name_, v)) {
        to.set(name_, v);
    }
}

template <class T>
void SimpleOption<T>::copy(const Configuration& from, Configured& to) const {
    Option::copy<T>(this->name(), from, to);
}

template <>
inline void SimpleOption<bool>::print(std::ostream& out) const {
    out << "   --" << name_ << " (" << description_ << ")";
}

template <class T>
void SimpleOption<T>::print(std::ostream& out) const {
    out << "   --" << this->name() << "=" << implementation_detail::Title<T>()() << " (" << this->description() << ")";
}

}  // namespace eckit::option
