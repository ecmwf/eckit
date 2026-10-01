// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File HttpServer.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef HttpServer_H
#define HttpServer_H

#include "eckit/thread/ThreadControler.h"
#include "eckit/web/HtmlResource.h"


namespace eckit {

//-----------------------------------------------------------------------------


class Url;

class HttpServer : public HtmlResource {
public:

    // -- Contructors

    HttpServer(int port, bool visible = false);

    // -- Destructor

    ~HttpServer();

private:

    // No copy allowed

    HttpServer(const HttpServer&);
    HttpServer& operator=(const HttpServer&);

    // -- Members

    eckit::ThreadControler http_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif
