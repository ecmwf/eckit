/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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
