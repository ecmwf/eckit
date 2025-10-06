/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
