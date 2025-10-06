/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/container/Trie.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


template <class T>
Trie<T>::Trie() : from_(0), set_(0), kids_(0) {}


template <class T>
Trie<T>::~Trie() {

    for (size_t i = 0; i < kids_.size(); i++) {
        if (kids_[i])
            delete (kids_[i]);
    }
}

template <class T>
void Trie<T>::insert(const std::string& key, T value) {
    const char* k = key.c_str();
    Trie<T>* x    = find((unsigned char*)k, true);
    x->value_     = value;
    x->set_       = true;
}

template <class T>
void Trie<T>::remove(const std::string& key) {
    const char* k = key.c_str();
    remove((unsigned char*)k);
}

template <class T>
T* Trie<T>::find(const std::string& key) const {
    const char* k = key.c_str();
    Trie<T>* x    = const_cast<Trie<T>*>(this)->find((unsigned char*)k, false);
    return (x && x->set_) ? &x->value_ : 0;
}

template <class T>
bool Trie<T>::contains(const std::string& key) const {
    return find(key) != 0;
}

template <class T>
bool Trie<T>::remove(const unsigned char* key) {
    if (*key == 0) {
        set_ = 0;
        return (kids_.size() == 0);  // Remove me if size if 0
    }

    int pos = int(*key) - int(from_);

    if (pos >= 0 && pos < int(kids_.size()) && kids_[pos]) {
        if (kids_[pos]->remove(key + 1)) {
            delete kids_[pos];
            kids_[pos] = 0;

            // May be we should shrink the list here

            for (size_t i = 0; i < kids_.size(); i++)
                if (kids_[i] != 0)
                    return false;

            // Delete me if I am not also a value
            kids_.clear();
            return !set_;
        }
    }

    return false;
}

template <class T>
Trie<T>* Trie<T>::find(const unsigned char* key, bool make) {
    if (*key == 0) {
        return this;
    }

    int pos = int(*key) - int(from_);

    if (pos >= 0 && pos < int(kids_.size()) && kids_[pos]) {
        return kids_[pos]->find(key + 1, make);
    }

    if (make) {

        // If outside of range, we need to expand the storage vector.
        if (pos < 0 || pos >= int(kids_.size())) {

            int from = 0;

            size_t sz;
            if (kids_.empty())  // First in
            {
                sz    = 1;
                from_ = *key;
            }
            else if (pos < 0)  // Insert before
            {
                sz    = kids_.size() - pos;
                from_ = *key;
                from  = -pos;
            }
            else  // Insert after
            {
                sz = pos + 1;
            }

            std::vector<Trie<T>*> kids(sz, 0);
            std::copy(kids_.begin(), kids_.end(), kids.begin() + from);

            kids_ = kids;
            pos   = int(*key) - int(from_);
        }


        ASSERT(kids_[pos] == 0);
        Trie<T>* k = new Trie<T>;
        kids_[pos] = k;
        return k->find(key + 1, make);
    }

    return 0;
}

template <class T>
void Trie<T>::print(std::ostream& s) const {
    if (set_)
        s << "(" << value_ << ")";

    for (size_t i = 0; i < kids_.size(); i++) {
        if (kids_[i]) {
            s << (unsigned char)(from_ + i) << ' ';
            kids_[i]->print(s);
        }
    }

    if (kids_.empty())
        s << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
