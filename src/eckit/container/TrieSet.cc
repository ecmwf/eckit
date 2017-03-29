/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/container/TrieSet.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


TrieSet::TrieSet() {}


TrieSet::~TrieSet() {}

void TrieSet::insert(const std::string& key) {

    trie_.insert(key, true);
}

void TrieSet::remove(const std::string& key) {

    trie_.remove(key);
}

bool TrieSet::exists(const std::string& key) const {

    bool* elem = trie_.find(key);

    if (elem)
        return *elem;
    return false;
}

void TrieSet::print(std::ostream& s) const {
    s << trie_;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
