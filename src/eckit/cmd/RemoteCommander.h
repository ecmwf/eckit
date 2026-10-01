// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RemoteCommander.h
/// @author Manuel Fuentes
/// @date   Jul 1996

#ifndef eckit_cmd_RemoteCommander_H
#define eckit_cmd_RemoteCommander_H

#include "eckit/net/NetService.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class RemoteCommander : public net::NetService {
public:

    // -- Contructors

    RemoteCommander(int);

    // -- Destructor

    ~RemoteCommander();

private:

    // -- Overridden methods

    // From net::NetService

    virtual net::NetUser* newUser(net::TCPSocket&) const;
    virtual std::string name() const { return "monitor"; }
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
