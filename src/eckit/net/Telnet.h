// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Telnet.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef eckit_Telnet_h
#define eckit_Telnet_h

#include "eckit/net/NetService.h"

namespace eckit::net {

class Telnet : public NetService {
public:

    // -- Contructors

    Telnet(int);

    // -- Destructor

    ~Telnet();

private:

    // No copy allowed

    Telnet(const Telnet&);
    Telnet& operator=(const Telnet&);

    // -- Overridden methods

    // From NetService

    virtual NetUser* newUser(net::TCPSocket&) const;
    virtual std::string name() const { return "telnet"; }
};

}  // namespace eckit::net

#endif
