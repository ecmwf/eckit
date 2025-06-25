/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Counted.h
/// @date Jun 1996
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_Counted_h
#define eckit_Counted_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace memory::detail {

class ThreadedLock {
public:

    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }

    mutable Mutex mutex_;
};

class NoLock {
public:

    void lock() const {}
    void unlock() const {}
};

}  // namespace memory::detail

//----------------------------------------------------------------------------------------------------------------------

/// Reference counting objects
/// Subclass from this class if you want reference counting object.
/// @note Remember to use 'virtual' inheritance in case of multiple inheritance

class Counted : private NonCopyable, private memory::detail::ThreadedLock {
public:  // methods

    void attach() const {
        lock();
        count_++;
        unlock();
    }

    void detach() const {
        lock();
        if (--count_ == 0) {
            unlock();
            delete this;
        }
        else {
            unlock();
        }
    }

    size_t count() const { return count_; }

    void lock() const { memory::detail::ThreadedLock::lock(); }

    void unlock() const { memory::detail::ThreadedLock::unlock(); }

public:

    Counted() : count_(0) {}

    virtual ~Counted();

private:  // members

    mutable size_t count_;
};

//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit

#endif
