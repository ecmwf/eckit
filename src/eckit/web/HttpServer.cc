// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/HttpServer.h"

#include "eckit/web/HttpService.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


HttpServer::HttpServer(int port, bool visible) : HtmlResource("/"), http_(new HttpService(port, visible)) {
    http_.start();
}

HttpServer::~HttpServer() {
    http_.stop();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
