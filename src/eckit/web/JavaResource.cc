/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <fstream>

#include "eckit/filesystem/PathName.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/JavaResource.h"
#include "eckit/web/Url.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


JavaResource::JavaResource() :
    HtmlResource("/java") {}

JavaResource::~JavaResource() {}

void JavaResource::GET(std::ostream& s, Url& url) {
    url.dontCache();

    eckit::PathName path("~/http/" + url.name());

    std::ifstream in(path.localPath());
    if (!in) {
        url.status(HttpError::NOT_FOUND);  // Not Found
        s << path << ": " << Log::syserr << std::endl;
    }
    else {
        //	(url.headerOut()).type("image/gif");

        s << HttpStream::dontEncode;
        char c;
        while (in.get(c))
            s << c;
        s << HttpStream::doEncode;
    }
}

static JavaResource javaResourceInstance;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
