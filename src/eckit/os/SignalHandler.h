/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_SignalHandler_h
#define eckit_SignalHandler_h

#include <setjmp.h>
#include <csignal>

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @warning This class has not been widely tested, and we don't
//           know if they delete objects properly when the signal is caugth

class SignalHandler : private NonCopyable {

public:  // methods

    enum Signal {
        SigInt  = 2,
        SigQuit = 3
    };

    // -- Contructors

    SignalHandler(void (*)(int) = interrupt, Signal = SigInt);

    // -- Destructor

    ~SignalHandler();

    static void checkInterrupt();

private:  // methods

    static void interrupt(int);

private:  // members

    int signal_;

    // unused // sigjmp_buf       buf_;
    struct sigaction save_;

    SignalHandler* next_;
    static SignalHandler* current_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
