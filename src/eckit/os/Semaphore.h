/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Semaphore.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Semaphore_h
#define eckit_Semaphore_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"
#include "eckit/filesystem/PathName.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Semaphore : private NonCopyable {

public: // methods

    /// Contructors

    Semaphore(const PathName& name,int count = 1);

    /// Destructor

    ~Semaphore();

    void lock(void);
    void unlock(void);
    bool test(unsigned short n = 0);

    int  get(int n = 0) const;
    void set(int, int n = 0);

    void raise(unsigned short n = 0);
    void lower(unsigned short n = 0);

    pid_t getpid()  const;

protected: // members

    int semaphore_;
    int count_;
    int level_;

    Mutex mutex_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
