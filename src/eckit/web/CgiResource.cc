// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/CgiResource.h"

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdPipe.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/Url.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CgiResource::CgiResource() : HtmlResource("/cgi") {}

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

    while (fgets(line, sizeof(line), pipe)) {
        s << line;
    }

    s << HttpStream::doEncode;
}

static CgiResource cgiResourceInstance;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
