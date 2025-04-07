/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Nov 2016


#ifndef eckit_StaticMutex_h
#define eckit_StaticMutex_h

#include <pthread.h>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Class meant to be used only for static mutexes protecting local resources inside a single compilation unit

class StaticMutex : private NonCopyable {

public:  // methods

    StaticMutex();

    ~StaticMutex();

    void lock();
    void unlock();

protected:  // members

    void init();

    /// since this will be static memory, it should be initialized to zero by the system

    pthread_mutex_t mutex_;
    bool exists_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
