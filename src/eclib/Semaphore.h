/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Semaphore.h
// Baudouin Raoult - ECMWF May 96

#ifndef eclib_Semaphore_h
#define eclib_Semaphore_h

#include "eclib/NonCopyable.h"
#include "eclib/Mutex.h"
#include "eclib/PathName.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class Semaphore : private NonCopyable {

public: // methods

    /// Contructors

    Semaphore(const PathName& name,int count = 1);

    /// Destructor

    ~Semaphore();

    void lock(void);
    void unlock(void);
    bool test(int n = 0);

    int  get(int n = 0) const;
    void set(int, int n = 0);

    void raise(int n = 0);
    void lower(int n = 0);

    pid_t getpid()  const;

protected: // members

    int semaphore_;
    int count_;
    int level_;

    Mutex mutex_;

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
