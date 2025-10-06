/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Factory.h
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Jul 2014


#pragma once

#include <iomanip>
#include <map>
#include <vector>

#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//------------------------------------------------------------------------------------------------------

template <class T>
class Factory {
public:

    // -- Types

    using product_t = T;
    using builder_t = typename product_t::builder_t;
    using key_t     = std::string;
    using storage_t = std::map<key_t, builder_t*>;

    // -- Constructors

    Factory(const Factory&) = delete;
    Factory(Factory&&)      = delete;

    // -- Operators

    void operator=(const Factory&) = delete;
    void operator=(Factory&&)      = delete;

    // -- Methods

    /// @return the instance of this singleton factory
    static Factory<T>& instance();

    /// @returns class name of the type built by this factory
    static std::string build_type() { return T::className(); }

    /// Checks if a builder is registered
    /// @param name of the builder
    bool exists(const key_t&) const;

    /// Registers a builder
    /// @param builder pointer
    /// @throw BadParameter if the builder already registered
    void regist(const key_t&, builder_t*);

    /// Remove a registered builder
    /// @throw BadParameter if the builder is not registered
    void unregist(const key_t&);

    /// Gets the builder registered to the associated key
    /// @param name of the builder
    const builder_t& get(const key_t&) const;

    /// @returns the number of builders registered to the factory
    size_t size() const;

    /// @returns the builder keys registered to the factory
    std::vector<key_t> keys() const;

private:

    // -- Constructors

    Factory() = default;

#if eckit_HAVE_ECKIT_MEMORY_FACTORY_EMPTY_DESTRUCTION
    // -- Destructor
    ~Factory() { ASSERT(store_.empty()); }
#endif

    // -- Members

    mutable Mutex mutex_;  ///< mutex protecting Factory singleton
    storage_t store_;      ///< storage for the builders in a map indexed by key_t

    // -- Methods

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Factory<T>& o) {
        o.print(os);
        return os;
    }
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
void Factory<T>::regist(const key_t& k, builder_t* b) {
    AutoLock<Mutex> lock(mutex_);
    ASSERT(b != nullptr);
    if (exists(k)) {
        throw BadParameter("Factory(" + build_type() + ") has already a builder for " + k, Here());
    }
    store_[k] = b;
}

template <class T>
void Factory<T>::unregist(const key_t& k) {
    AutoLock<Mutex> lock(mutex_);
    if (!exists(k)) {
        throw BadParameter("Factory(" + build_type() + ") has no builder for " + k, Here());
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
        throw BadParameter("Factory(" + build_type() + ") has no builder for " + k, Here());
    }
    return *(store_.find(k)->second);
}

template <class T>
void Factory<T>::print(std::ostream& os) const {
    AutoLock<Mutex> lock(mutex_);
    os << "Factory(" << build_type() << ")" << std::endl;

    int key_width = 0;
    for (const auto& i : store_) {
        key_width = std::max(static_cast<int>(i.first.size()), key_width);
    }

    for (const auto& i : store_) {
        os << "    " << std::setw(key_width) << std::left << i.first << "  --  " << i.second << std::endl;
    }
}

template <class T>
std::vector<typename Factory<T>::key_t> Factory<T>::keys() const {
    AutoLock<Mutex> lock(mutex_);

    std::vector<key_t> keysv;
    keysv.reserve(store_.size());
    for (const auto& i : store_) {
        keysv.push_back(i.first);
    }
    return keysv;
}

//------------------------------------------------------------------------------------------------------

}  // namespace eckit
