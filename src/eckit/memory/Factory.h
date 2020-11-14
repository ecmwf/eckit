/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_memory_Factory_h
#define eckit_memory_Factory_h

/// @file Factory.h
/// @author Tiago Quintino
/// @date Jul 2014

#include <iomanip>
#include <map>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

//------------------------------------------------------------------------------------------------------

template <class T>
class Factory {

public:  // types
    typedef std::string key_t;

    typedef T product_t;
    typedef typename product_t::builder_t builder_t;
    typedef builder_t* builder_ptr;

    typedef std::map<key_t, builder_ptr> storage_t;

public:  // methods
    /// @return the instance of this singleton factory
    static Factory<T>& instance();

    /// @returns class name of the type built by this factory
    static std::string build_type() { return T::className(); }

    /// Checks if a builder is registered
    /// @param name of the builder
    bool exists(const key_t& k) const;

    /// Registers a builder
    /// @param builder pointer
    /// @throw BadParameter if the builder already registered
    void regist(const key_t&, builder_ptr);

    /// Remove a registered builder
    /// @throw BadParameter if the builder is not registered
    void unregist(const key_t&);

    /// Gets the builder registered to the associated key
    /// @param name of the builder
    const builder_t& get(const key_t& name) const;

    /// @returns the number of builders registered to the factory
    size_t size() const;

    std::vector<key_t> keys() const;

    friend std::ostream& operator<<(std::ostream& os, const Factory<T>& o) {
        o.print(os);
        return os;
    }

private:  // methods
    void print(std::ostream&) const;

    Factory() {
        // std::cout << "Building Factory of " << build_type() << std::endl;
    }

    ~Factory() {
        //		std::cout << "Destroying Factory of " << build_type() << std::endl;
        //		if( store_.size() != 0 )
        //		{
        //			std::cout << "WARNING : Factory of " << build_type() << " still has " << size() << " providers" <<
        // std::endl; 			std::cout << *this << std::endl;
        //		}

        ASSERT(store_.size() == 0);
    }

private:                   // members
    mutable Mutex mutex_;  ///< mutex protecting Factory singleton
    storage_t store_;      ///< storage for the builders in a map indexed by key_t
};

//------------------------------------------------------------------------------------------------------

template <class T>
Factory<T>& Factory<T>::instance() {
    static Factory<T> obj;
    return obj;
}

template <class T>
bool Factory<T>::exists(const key_t& k) const {
    AutoLock<Mutex> lock(mutex_);
    return (store_.find(k) != store_.end());
}

template <class T>
void Factory<T>::regist(const key_t& k, builder_ptr b) {
    AutoLock<Mutex> lock(mutex_);
    ASSERT(b);
    if (exists(k)) {
        throw BadParameter("Factory of " + build_type() + " has already a builder for " + k, Here());
    }
    store_[k] = b;
}

template <class T>
void Factory<T>::unregist(const key_t& k) {
    AutoLock<Mutex> lock(mutex_);
    if (!exists(k)) {
        throw BadParameter("Factory of " + build_type() + " has no builder for " + k, Here());
    }
    store_.erase(k);
}

template <class T>
size_t Factory<T>::size() const {
    AutoLock<Mutex> lock(mutex_);
    return store_.size();
}

template <class T>
const typename Factory<T>::builder_t& Factory<T>::get(const key_t& k) const {
    AutoLock<Mutex> lock(mutex_);
    if (!exists(k)) {
        throw BadParameter("Factory of " + build_type() + " has no builder for " + k, Here());
    }
    return *store_.find(k)->second;
}

template <class T>
void Factory<T>::print(std::ostream& os) const {
    AutoLock<Mutex> lock(mutex_);
    os << "Factory(" << build_type() << ")" << std::endl;

    size_t key_width = 0;
    for (typename storage_t::const_iterator i = store_.begin(); i != store_.end(); ++i) {
        key_width = std::max(i->first.size(), key_width);
    }

    for (typename storage_t::const_iterator i = store_.begin(); i != store_.end(); ++i) {
        os << "    " << std::setw(key_width) << std::left << i->first << "  --  " << (*(*i).second) << std::endl;
    }
}

template <class T>
std::vector<typename Factory<T>::key_t> Factory<T>::keys() const {
    AutoLock<Mutex> lock(mutex_);

    std::vector<key_t> keysv;
    keysv.reserve(store_.size());
    for (typename storage_t::const_iterator i = store_.begin(); i != store_.end(); ++i) {
        keysv.push_back(i->first);
    }
    return keysv;
}

//------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif  // eckit_memory_Factory_h
