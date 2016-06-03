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
/// @author Simon Smart
/// @date March 2016

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/option/VectorOption.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/utils/Translator.h"
#include "eckit/types/Types.h"


namespace eckit {

namespace option {

//----------------------------------------------------------------------------------------------------------------------


template<class T>
VectorOption<T>::VectorOption(const std::string &name, const std::string &description, size_t size):
    Option(name, description), size_(size) {
}

template<class T>
VectorOption<T>::~VectorOption() {
}

template<class T>
void VectorOption<T>::set(const std::string &value, Configured &parametrisation) const {
    eckit::Translator<std::string, T> t;

    eckit::Tokenizer parse("/");
    std::vector<std::string> v;
    parse(value, v);

    std::vector<T> values;
    for(size_t i = 0; i < v.size(); i++) {
        values.push_back(t(v[i]));
    }

    if (values.size() != size_)
        throw UserError(std::string("Size of supplied vector \"") + name_ + "\" incorrect", Here());

    parametrisation.set(name_, values);
}

template<class T>
void VectorOption<T>::print(std::ostream &out) const {
    out << "   --" << name_;

    const char *sep = "=";
    for(size_t i = 0; i < size_; i++) {
        out << sep  << Title<T>()();
        sep = "/";
    }

    out << " (" << description_ << ")";
}


template<class T>
void VectorOption<T>::copy(const Configuration &from, Configured &to) const {
    std::vector<T> v;
    if(!from.get(name_, v)) {
        throw eckit::SeriousBug("Cannot get value for " + name_);
    }
    to.set(name_, v);
}


//----------------------------------------------------------------------------------------------------------------------


} // namespace option

} // namespace eckit

