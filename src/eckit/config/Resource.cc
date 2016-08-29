/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/config/Resource.h"
#include "eckit/utils/Translator.h"
#include "eckit/parser/Tokenizer.h"

namespace eckit {
template<class T> void Resource<T>::setValue(const std::string& s)
{
    value_ = Translator<std::string, T>()(s);
}

template<class T> std::string Resource<T>::getValue() const
{
    return Translator<T, std::string>()(value_);
}


template<class T>
Resource<T>::Resource(const std::string& str, const std::string& value, bool tokenize):
    ResourceBase(0, str) {
    std::vector<std::string> v;
    Tokenizer parse(",");

    parse(value, v);

    for (std::vector<std::string>::const_iterator j = v.begin(); j != v.end(); ++j) {
        value_.push_back(Translator<std::string, T>()(*j));
    }
}

} // namespace
