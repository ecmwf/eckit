// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Telnetable.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_Telnetable_h
#define eckit_Telnetable_h

#include "eckit/thread/ThreadControler.h"

namespace eckit::net {

/// A telnet-able object

class Telnetable {
public:

    // -- Contructors

    Telnetable(int port);

    // -- Destructor

    ~Telnetable();

private:

    // No copy allowed

    Telnetable(const Telnetable&);
    Telnetable& operator=(const Telnetable&);

    // -- Members

    ThreadControler telnet_;
};

}  // namespace eckit::net

#endif
