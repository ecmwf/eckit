// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
