// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 96

#ifndef eckit_Thread_h
#define eckit_Thread_h

#include "eckit/thread/Mutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// This should be a Task

class Thread {
public:

    friend class ThreadControler;

    Thread(bool autodel = true);

    Thread(const Thread&)            = delete;
    Thread& operator=(const Thread&) = delete;
    Thread(Thread&&)                 = delete;
    Thread& operator=(Thread&&)      = delete;

    virtual ~Thread();

    void stop();

protected:  // members

    Mutex mutex_;

protected:  // methods

    bool stopped();

private:  // members

    bool stop_;
    bool autodel_;

private:  // methods

    virtual void run() = 0;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
