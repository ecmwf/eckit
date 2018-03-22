/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ProxiedTCPServer.h
// Baudouin Raoult - ECMWF Aug 2017

#ifndef eckit_ProxiedTCPServer_h
#define eckit_ProxiedTCPServer_h

#include "eckit/net/TCPServer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// AIX #define accept to naccept. It took me a while :-(
// this clashes with the method accept()
// so I need to include socket here so all client of this
// class actually see this #define. So much for encapsulation.

class ProxiedTCPServer : public TCPServer {
public:

// -- Contructors

    ProxiedTCPServer(int port = 0, const std::string& addr = "");


// -- Destructor

    ~ProxiedTCPServer();

// -- Methods


    // accept a client, more can be accepted

    virtual TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0, bool* connected = 0);

// -- Overridden methods



protected:

// -- Members


// -- Overridden methods


private:

// No copy allowed

    ProxiedTCPServer(const ProxiedTCPServer&);
    ProxiedTCPServer& operator=(const ProxiedTCPServer&);

    //
    virtual void print(std::ostream& s) const;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
