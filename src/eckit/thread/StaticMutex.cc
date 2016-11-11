/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/StaticMutex.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static void init_mutex_attr(pthread_mutex_t* mutex) {

    pthread_mutexattr_t attr;

    CHECK_CALL_NOLOG( ::pthread_mutexattr_init(&attr) );
    CHECK_CALL_NOLOG( ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) );

    CHECK_CALL_NOLOG( ::pthread_mutex_init(mutex, &attr) );

    CHECK_CALL_NOLOG(::pthread_mutexattr_destroy(&attr));
}

struct StaticMutexRegister {

    // ensures creation on first call as in pthread_once_t
    // but without being dependent on order of static initialization
    static StaticMutexRegister& instance();

    StaticMutexRegister();

    void add(pthread_mutex_t* mutex) {
        CHECK_CALL_NOLOG(::pthread_mutex_lock(&register_lock));
        set_.insert(mutex);
        CHECK_CALL_NOLOG(::pthread_mutex_unlock(&register_lock));
    }

    void remove(pthread_mutex_t* mutex) {
        CHECK_CALL_NOLOG(::pthread_mutex_lock(&register_lock));
        set_.erase(mutex);
        CHECK_CALL_NOLOG(::pthread_mutex_unlock(&register_lock));
    }

    pthread_mutex_t register_lock;
    std::set<pthread_mutex_t*> set_;
};

static void get_locks()
{
    std::set<pthread_mutex_t*>& reg = StaticMutexRegister::instance().set_;
    for(std::set<pthread_mutex_t*>::iterator i = reg.begin(); i != reg.end(); ++i) {
        CHECK_CALL_NOLOG(::pthread_mutex_lock(*i));
    }
}

static void release_locks_parent()
{
    std::set<pthread_mutex_t*>& reg = StaticMutexRegister::instance().set_;
    for(std::set<pthread_mutex_t*>::reverse_iterator i = reg.rbegin(); i != reg.rend(); ++i) {
        CHECK_CALL_NOLOG(::pthread_mutex_unlock(*i));
    }
}

static void release_locks_child()
{
    std::set<pthread_mutex_t*>& reg = StaticMutexRegister::instance().set_;
    for(std::set<pthread_mutex_t*>::reverse_iterator i = reg.rbegin(); i != reg.rend(); ++i) {
        init_mutex_attr(*i);
    }
}

StaticMutexRegister& StaticMutexRegister::instance()
{
    static StaticMutexRegister reg;
    return reg;
}

StaticMutexRegister::StaticMutexRegister() {

    init_mutex_attr(&register_lock);

    // see ECKIT-140
    CHECK_CALL_NOLOG(::pthread_atfork(get_locks, release_locks_parent, release_locks_child));
}

//----------------------------------------------------------------------------------------------------------------------

StaticMutex::StaticMutex() :
    exists_(false)
{
    init_mutex_attr(&mutex_);
    exists_ = true;

    StaticMutexRegister::instance().add(&mutex_);
}

StaticMutex::~StaticMutex()
{
    StaticMutexRegister::instance().remove(&mutex_);
    CHECK_CALL_NOLOG(::pthread_mutex_destroy(&mutex_));
}

void StaticMutex::lock()
{
	if(!exists_)
	{
        std::cerr << "StaticMutex used before being constructed" << std::endl;
        ::abort();
	}

    CHECK_CALL_NOLOG(::pthread_mutex_lock(&mutex_));
}

void StaticMutex::unlock()
{
	if(!exists_)
	{
        std::cerr << "StaticMutex used before being constructed" << std::endl;
        ::abort();
	}

    CHECK_CALL_NOLOG(::pthread_mutex_unlock(&mutex_));
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

