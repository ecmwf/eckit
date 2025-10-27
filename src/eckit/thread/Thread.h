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
/// @date   Jun 96

#ifndef eckit_Thread_h
#define eckit_Thread_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// This should be a Task

class Thread : private NonCopyable {
public:

    friend class ThreadControler;

    Thread(bool autodel = true);

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
