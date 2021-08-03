/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Tokenizer.h"

#include <iostream>
#include <iterator>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class Container>
void tokenizeInsert(const std::set<char, std::less<char> >& separator, const std::string& raw,
                    std::insert_iterator<Container> ins, bool keepEmpty) {
    int index         = 0;
    int length        = raw.length();
    std::string token = "";

    while (index < length) {
        char c = raw[index];

        if (separator.find(c) != separator.end()) {
            if (token.length() > 0 || keepEmpty) {
                ins = token;
            }
            token = "";
        }
        else
            token += c;

        index++;
    }

    if (token.length() > 0 || keepEmpty) {
        ins = token;
    }
}

//----------------------------------------------------------------------------------------------------------------------


Tokenizer::Tokenizer(char c, bool keepEmpty) :
    keepEmpty_(keepEmpty) {
    separator_.insert(c);
}

Tokenizer::Tokenizer(const std::string& separators, bool keepEmpty) :
    keepEmpty_(keepEmpty) {
    for (std::string::size_type i = 0; i < separators.length(); i++)
        separator_.insert(separators[i]);
}

Tokenizer::~Tokenizer() {}

void Tokenizer::operator()(const std::string& raw, std::vector<std::string>& v) const {
    tokenizeInsert(separator_, raw, std::inserter(v, v.end()), keepEmpty_);
}

void Tokenizer::operator()(std::istream& in, std::vector<std::string>& v) const {
    std::string raw;
    char c;

    while (in.get(c) && c != EOF && c != '\n')
        raw += c;

    tokenizeInsert(separator_, raw, std::inserter(v, v.end()), keepEmpty_);
}

void Tokenizer::operator()(const std::string& raw, std::set<std::string>& s) const {
    tokenizeInsert(separator_, raw, std::inserter(s, s.end()), keepEmpty_);
}

void Tokenizer::operator()(std::istream& in, std::set<std::string>& s) const {
    std::string raw;
    char c;

    while (in.get(c) && c != EOF && c != '\n')
        raw += c;

    tokenizeInsert(separator_, raw, std::inserter(s, s.end()), keepEmpty_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
