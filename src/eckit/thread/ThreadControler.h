// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
