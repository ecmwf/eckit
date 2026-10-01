// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File JavaUser.h
// Baudouin Raoult - ECMWF Apr 97

#ifndef JavaUser_H
#define JavaUser_H

#include "eckit/net/NetUser.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class JavaUser : public eckit::net::NetUser {
public:

    JavaUser(eckit::net::TCPSocket&);

    ~JavaUser();

private:

    virtual void serve(eckit::Stream&, std::istream&, std::ostream&);
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
