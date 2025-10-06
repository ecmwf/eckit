/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClassExtent.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_ClassExtent_h
#define eckit_ClassExtent_h

#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//-----------------------------------------------------------------------------

template <class T>
class ClassExtent : private NonCopyable {
public:

    // -- Contructors

    ClassExtent(T*);

    // -- Destructor

    ~ClassExtent();

    // -- Methods

    static size_t size();

public:  // methods

    static void callAll(void (T::*)());
    static void callAll(void (T::*)() const);

    template <class P>
    static void callAll(void (T::*)(P), P);

    template <class P>
    static void callAll(void (T::*)(P) const, P);

    template <class P1, class P2>
    static void callAll(void (T::*)(P1, P2), P1, P2);

    template <class P>
    static void callAll(void (T::*)(P&) const, P&);

    template <class P>
    static void callAll(void (T::*)(P&), P&);

    template <class P1, class P2>
    static void callAll(void (T::*)(P1&, P2&), P1&, P2&);

private:  // members

    struct Extent {
        typedef std::map<ClassExtent<T>*, T*, std::less<ClassExtent<T>*> > Map;
        Mutex mutex_;
        Map map_;
        bool inited_;
        Extent();
        ~Extent();
    };

    static Extent extent_;
};

//-----------------------------------------------------------------------------

// We assume that global-initialisation is single threaded

template <class T>
typename ClassExtent<T>::Extent ClassExtent<T>::extent_;

template <class T>
ClassExtent<T>::ClassExtent(T* obj) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);
    extent_.map_[this] = obj;
}

template <class T>
ClassExtent<T>::~ClassExtent() {

    if (extent_.inited_)  // This can be after exit() is called
                          // I need to find a solution
    {

        ASSERT(extent_.inited_);

        AutoLock<Mutex> lock(extent_.mutex_);
        ASSERT(extent_.map_.find(this) != extent_.map_.end());
        extent_.map_.erase(this);
    }
}

template <class T>
size_t ClassExtent<T>::size() {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);
    return extent_.map_.size();
}

template <class T>
void ClassExtent<T>::callAll(void (T::*proc)()) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = map.begin(); i != map.end(); ++i) {
        ((*i).second->*proc)();
    }
}

template <class T>
void ClassExtent<T>::callAll(void (T::*proc)() const) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = extent_.map_.begin(); i != extent_.map_.end(); ++i) {
        ((*i).second->*proc)();
    }
}

template <class T>
template <class P>
void ClassExtent<T>::callAll(void (T::*proc)(P), P arg) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = map.begin(); i != map.end(); ++i) {
        ((*i).second->*proc)(arg);
    }
}

template <class T>
template <class P>
void ClassExtent<T>::callAll(void (T::*proc)(P) const, P arg) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = extent_.map_.begin(); i != extent_.map_.end(); ++i) {
        ((*i).second->*proc)(arg);
    }
}

template <class T>
template <class P1, class P2>
void ClassExtent<T>::callAll(void (T::*proc)(P1, P2), P1 arg1, P2 arg2) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = map.begin(); i != map.end(); ++i) {
        ((*i).second->*proc)(arg1, arg2);
    }
}

template <class T>
template <class P>
void ClassExtent<T>::callAll(void (T::*proc)(P&), P& arg) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = extent_.map_.begin(); i != extent_.map_.end(); ++i) {
        ((*i).second->*proc)(arg);
    }
}

template <class T>
template <class P>
void ClassExtent<T>::callAll(void (T::*proc)(P&) const, P& arg) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = extent_.map_.begin(); i != extent_.map_.end(); ++i) {
        ((*i).second->*proc)(arg);
    }
}

template <class T>
template <class P1, class P2>
void ClassExtent<T>::callAll(void (T::*proc)(P1&, P2&), P1& arg1, P2& arg2) {
    ASSERT(extent_.inited_);
    AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
    // for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
    typedef typename ClassExtent<T>::Extent::Map map_type;
    typename map_type::iterator i;
    for (i = map.begin(); i != map.end(); ++i) {
        ((*i).second->*proc)(arg1, arg2);
    }
}

template <class T>
ClassExtent<T>::Extent::Extent() : inited_(true) {}

template <class T>
ClassExtent<T>::Extent::~Extent() {
    inited_ = false;
}

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
