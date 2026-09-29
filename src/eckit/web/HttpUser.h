// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File HttpUser.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef HttpUser_H
#define HttpUser_H

#include "eckit/net/NetUser.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class HttpUser : public eckit::net::NetUser {
public:

    HttpUser(eckit::net::TCPSocket&);
    ~HttpUser() override;

private:

    void serve(eckit::Stream&, std::istream&, std::ostream&) override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
