/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Jun 96

#ifndef eckit_TCPServer_h
#define eckit_TCPServer_h

#include "eckit/net/TCPSocket.h"



namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// AIX #define accept to naccept. It took me a while :-(
// this clashes with the method accept()
// so I need to include socket here so all client of this
// class actually see this #define. So much for encapsulation.

class TCPServer : public TCPSocket {
public:

// -- Contructors

    TCPServer(int port = 0, const std::string& addr = "", bool reusePort = false);


// -- Destructor

    ~TCPServer();

// -- Methods

    void willFork(bool);


    // accept a client, more can be accepted

    virtual TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0, bool* connected = 0);
    void closeExec(bool on) { closeExec_ = on; }

// -- Overridden methods

    // From TCPSocket

    virtual int socket();

    virtual void close();


protected:

// -- Members

    int port_;
    int listen_;
    std::string addr_;

// -- Overridden methods

    // From TCPSocket

    virtual void bind();

protected:

    virtual void print(std::ostream& s) const;

private:

// No copy allowed

    TCPServer(const TCPServer&);
    TCPServer& operator=(const TCPServer&);

    // To be used by Select

    virtual std::string bindingAddress() const;

    bool closeExec_;
    bool reusePort_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
