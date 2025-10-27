/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
