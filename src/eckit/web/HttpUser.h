/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
