// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <fstream>

#include "eckit/filesystem/PathName.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/JavaResource.h"
#include "eckit/web/Url.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


JavaResource::JavaResource() : HtmlResource("/java") {}

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
        while (in.get(c)) {
            s << c;
        }
        s << HttpStream::doEncode;
    }
}

static JavaResource javaResourceInstance;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
