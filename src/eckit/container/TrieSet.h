/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file trie.h
/// @author Baudouin Raoult
/// @author Simon Smart
/// @date March 2017


#ifndef eckit_containers_TrieSet_H
#define eckit_containers_TrieSet_H

#include "eckit/container/Trie.h"

#include <ostream>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TrieSet {

public: // methods

    TrieSet();
    ~TrieSet();

    bool empty() const { return trie_.empty(); }

    void insert(const std::string& key);
    void remove(const std::string& key);
    bool exists(const std::string& key) const;

protected: // methods

	void print(std::ostream&) const; 	

private: // members

    Trie<bool> trie_;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const TrieSet& p) {
        p.print(s); return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit


#endif // eckit_containers_TrieSet_H
