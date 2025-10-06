/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JavaServer.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef JavaServer_H
#define JavaServer_H

#include "eckit/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class JavaServer {
public:

    // -- Contructors

    JavaServer(int port);

    // -- Destructor

    ~JavaServer();

private:

    // No copy allowed

    JavaServer(const JavaServer&);
    JavaServer& operator=(const JavaServer&);

    // -- Members

    eckit::ThreadControler java_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif
