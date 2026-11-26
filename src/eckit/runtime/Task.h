// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   May 96

#ifndef eckit_Task_h
#define eckit_Task_h

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Task {
public:

    Task();

    Task(const Task&)            = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&)                 = delete;
    Task& operator=(Task&&)      = delete;

    virtual ~Task();

    virtual void start()  = 0;
    virtual void stop()   = 0;
    virtual void wait()   = 0;
    virtual bool active() = 0;
    virtual void kill()   = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
