/*
 * (C) Copyright 1996- ECMWF.
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


#ifndef eckit_containers_Trie_H
#define eckit_containers_Trie_H

#include "eckit/memory/NonCopyable.h"

#include <ostream>
#include <vector>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class Trie : private NonCopyable {

public:  // methods

    Trie();
    ~Trie();

    bool empty() const { return kids_.empty() && !set_; }

    void insert(const std::string& key, T value);
    void remove(const std::string& key);
    bool contains(const std::string& key) const;
    T* find(const std::string& key) const;

protected:  // methods

    void print(std::ostream&) const;

private:  // members

    Trie<T>* find(const unsigned char*, bool);

    bool remove(const unsigned char* key);

private:  // members

    unsigned short from_;

    /// Do we have a value in the value_ (or is just uninintialised POD/default constructed).
    bool set_;

    std::vector<Trie<T>*> kids_;
    T value_;

private:  // friends

    friend std::ostream& operator<<(std::ostream& s, const Trie<T>& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#include "Trie.cc"

#endif  // eckit_containers_Trie_H
