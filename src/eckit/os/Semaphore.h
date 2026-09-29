// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   May 1996

#ifndef eckit_os_Semaphore_h
#define eckit_os_Semaphore_h

#include "eckit/filesystem/PathName.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Semaphore {

public:  // methods

    Semaphore(const PathName& name, int count = 1);

    Semaphore(const Semaphore&)            = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&&)                 = delete;
    Semaphore& operator=(Semaphore&&)      = delete;

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
