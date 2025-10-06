/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JavaService.h
// Baudouin Raoult - ECMWF Apr 97

#ifndef JavaService_H
#define JavaService_H

#include "eckit/net/NetService.h"


namespace eckit {


class JavaService : public eckit::net::NetService {
public:

    JavaService(int port);

    ~JavaService();

private:

    virtual eckit::net::NetUser* newUser(eckit::net::TCPSocket& socket) const;
    virtual std::string name() const { return "java"; }
};


}  // namespace eckit


#endif
