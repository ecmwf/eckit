// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_SignalHandler_h
#define eckit_SignalHandler_h

#include <setjmp.h>

#include <csignal>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @warning This class has not been widely tested, and we don't
//           know if they delete objects properly when the signal is caugth

class SignalHandler {

public:  // methods

    enum Signal {
        SigInt  = 2,
        SigQuit = 3
    };

    // -- Contructors

    SignalHandler(void (*)(int) = interrupt, Signal = SigInt);

    SignalHandler(const SignalHandler&)            = delete;
    SignalHandler& operator=(const SignalHandler&) = delete;
    SignalHandler(SignalHandler&&)                 = delete;
    SignalHandler& operator=(SignalHandler&&)      = delete;

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
