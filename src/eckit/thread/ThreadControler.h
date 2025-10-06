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
/// @date   May 96

#ifndef eckit_ThreadControler_h
#define eckit_ThreadControler_h

#include "eckit/runtime/Task.h"
#include "eckit/thread/MutexCond.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Thread;

/// @note Don't subclass from ThreadControler but from Thread
class ThreadControler : public Task {
public:

    /// @note ThreadControler takes ownership of Thread
    explicit ThreadControler(Thread*, bool detached = true, size_t stack = 0);

    ~ThreadControler() override;

    void start() override;
    void stop() override;
    void kill() override;
    void wait() override;
    bool active() override;

protected:  // members

    MutexCond cond_;
    bool detached_;

private:  // members

    pthread_t thread_;
    Thread* proc_;
    size_t stack_;
    bool running_;

private:  // methods

    void execute();

    static void* startThread(void* data);
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
