// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015

#pragma once

#include <iostream>

#include "eckit/config/Configured.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/option/FactoryOption.h"

namespace eckit::option {


template <class T>
FactoryOption<T>::FactoryOption(const std::string& name, const std::string& description) : base_t(name, description) {}

template <class T>
FactoryOption<T>::FactoryOption(const std::string& name, const std::string& description, std::string default_value) :
    base_t(name, description, std::move(default_value)) {}

template <class T>
size_t FactoryOption<T>::set(Configured& parametrisation, [[maybe_unused]] size_t values, args_t::const_iterator begin,
                             args_t::const_iterator end) const {
    if (begin == end) {
        throw UserError("No option value found for FactoryOption, where 1 was expected");
    }
    auto value = translate(*begin);
    set_value(value, parametrisation);
    return 1;
}

template <class T>
void FactoryOption<T>::set_value(const std::string& value, Configured& parametrisation) const {
    parametrisation.set(name_, value);
}

template <class T>
std::string FactoryOption<T>::translate(const std::string& value) const {
    return value;
}

template <class T>
void FactoryOption<T>::copy(const Configuration& from, Configured& to) const {
    Option::copy<std::string>(name_, from, to);
}

template <class T>
void FactoryOption<T>::print(std::ostream& out) const {
    out << "   --" << name_ << "=name" << " (" << description_ << ")";
    out << std::endl << "     Values are: ";
    T::list(out);
}

}  // namespace eckit::option
