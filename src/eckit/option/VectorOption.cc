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
/// @author Simon Smart
/// @date March 2016

#pragma once

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/option/Title.h"
#include "eckit/option/VectorOption.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit::option {

template <class T>
VectorOption<T>::VectorOption(const std::string& name, const std::string& description, size_t size,
                              const char* separator) :
    base_t(name, description), size_(size), separator_(separator) {}

template <class T>
VectorOption<T>::VectorOption(const std::string& name, const std::string& description, size_t size,
                              const std::vector<T>& default_value, const char* separator) :
    base_t(Option(name, description, default_value)), size_(size), separator_(separator) {}


template <class T>
size_t VectorOption<T>::set(Configured& parametrisation, size_t values, args_t::const_iterator begin,
                            args_t::const_iterator end) const {
    if (begin == end) {
        throw UserError("No option value found for VectorOption, where 1 was expected");
    }
    // Take first value in range [begin, end)
    auto value = translate(*begin);
    set_value(value, parametrisation);
    return 1;
}

template <class T>
void VectorOption<T>::set_value(const std::vector<T>& value, Configured& parametrisation) const {
    parametrisation.set(this->name(), value);
}

template <class T>
std::vector<T> VectorOption<T>::translate(const std::string& value) const {
    Translator<std::string, T> t;

    Tokenizer parse(separator_);
    std::vector<std::string> tokens;
    parse(value, tokens);

    std::vector<T> values;
    for (size_t i = 0; i < tokens.size(); i++) {
        values.push_back(t(tokens[i]));
    }

    if (size_) {
        if (values.size() != size_)
            throw UserError(std::string("Size of supplied vector \"") + this->name() + "\" incorrect", Here());
    }

    return values;
}

template <class T>
void VectorOption<T>::print(std::ostream& out) const {
    out << "   --" << this->name();

    const char* sep = "=";
    for (size_t i = 0; i < (size_ ? size_ : 2); i++) {
        out << sep << implementation_detail::Title<T>()();
        sep = separator_;
    }

    if (size_ == 0) {
        out << sep << "...";
    }

    out << " (" << this->description() << ")";
}

template <class T>
void VectorOption<T>::copy(const Configuration& from, Configured& to) const {
    Option::copy<std::vector<T>>(this->name(), from, to);
}

}  // namespace eckit::option
