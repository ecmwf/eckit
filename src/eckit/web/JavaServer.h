// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
