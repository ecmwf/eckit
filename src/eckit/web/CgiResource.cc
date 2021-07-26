/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/web/CgiResource.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdPipe.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/Url.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CgiResource::CgiResource() :
    HtmlResource("/cgi") {}

CgiResource::~CgiResource() {}

void CgiResource::GET(std::ostream& s, Url& url) {
    eckit::PathName cgiScriptsPath = eckit::Resource<eckit::PathName>("cgiScriptsPath", "~/admin/cgi-scripts");
    eckit::PathName path(cgiScriptsPath / url.name());
    std::ostringstream cmd;

    std::string mode = url["parameter"];

    if (mode == "") {
        cmd << "env ";
        url.cgiParam(cmd, ' ');
        cmd << " " << path;
    }
    else {
        cmd << path << ' ';
        url.cgiParam(cmd, ' ');
    }

    StdPipe pipe(cmd.str(), "r");
    AutoCloser<StdPipe> closer(pipe);
    char line[1024];

    s << HttpStream::dontEncode;

    while (fgets(line, sizeof(line), pipe))
        s << line;

    s << HttpStream::doEncode;
}

static CgiResource cgiResourceInstance;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
