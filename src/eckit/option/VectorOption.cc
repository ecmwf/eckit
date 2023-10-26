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
/// @author Simon Smart
/// @date March 2016


#include "eckit/option/VectorOption.h"

#include <iostream>
#include <vector>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"


namespace eckit::option {


template <class T>
VectorOption<T>::VectorOption(const std::string& name, const std::string& description, size_t size,
                              const char* separator, bool keepEmpty) :
    Option(name, description), size_(size), separator_(separator), keepEmpty_(keepEmpty) {}


template <class T>
void VectorOption<T>::set(Configured& parametrisation) const {
    set({}, parametrisation);
}


template <class T>
void VectorOption<T>::set(const std::string& value, Configured& parametrisation) const {
    eckit::Translator<std::string, T> t;

    eckit::Tokenizer parse(separator_, keepEmpty_);
    std::vector<std::string> v;
    parse(value, v);

    std::vector<T> values;
    for (size_t i = 0; i < v.size(); i++) {
        values.push_back(t(v[i]));
    }

    if (size_ > 0 && values.size() != size_) {
        throw UserError("Size of supplied vector \"" + name() + "\" incorrect", Here());
    }

    parametrisation.set(name(), values);
}


template <class T>
void VectorOption<T>::print(std::ostream& out) const {
    out << "   --" << name();

    const char* sep = "=";
    for (size_t i = 0; i < (size_ ? size_ : 2); i++) {
        out << sep << Title<T>()();
        sep = separator_;
    }

    if (size_ == 0) {
        out << sep << "...";
    }

    out << " (" << description() << ")";
}


template <class T>
void VectorOption<T>::copy(const Configuration& from, Configured& to) const {
    std::vector<T> v;
    if (from.get(name(), v)) {
        to.set(name(), v);
    }
}


}  // namespace eckit::option
