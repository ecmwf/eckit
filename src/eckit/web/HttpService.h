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
/// @date  Oct 96

#ifndef HttpService_H
#define HttpService_H

#include "eckit/net/NetService.h"


namespace eckit {


class HttpService : public eckit::net::NetService {
public:

    explicit HttpService(int port, bool visible = false);

    ~HttpService();

private:

// -- Overridden methods

    // From net::NetService

    virtual eckit::net::NetUser* newUser(eckit::net::TCPSocket&);
    virtual std::string name() { return "http"; }

};


} // namespace eckit


#endif
