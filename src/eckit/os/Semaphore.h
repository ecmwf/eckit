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
/// @date   May 1996

#ifndef eckit_os_Semaphore_h
#define eckit_os_Semaphore_h

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Semaphore : private NonCopyable {

public:  // methods

    Semaphore(const PathName& name, int count = 1);

    ~Semaphore();

    void lock(void);
    void unlock(void);
    bool tryLock(void);
    bool test(unsigned short n = 0);

    int get(int n = 0) const;
    void set(int, int n = 0);

    void raise(unsigned short n = 0);
    void raise(unsigned short n, short v);
    void lower(unsigned short n = 0);
    void lower(unsigned short n, short v);

    pid_t getpid() const;

protected:  // members

    int semaphore_;
    int count_;
    int level_;

    Mutex mutex_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
