// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/web/JavaServer.h"
#include "eckit/web/JavaService.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


JavaServer::JavaServer(int port) : java_(new JavaService(port)) {
    java_.start();
}

JavaServer::~JavaServer() {
    java_.stop();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
