// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/FileResource.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/Url.h"


namespace eckit {


FileResource::FileResource() : HtmlResource("/files") {}

FileResource::~FileResource() {}

void FileResource::GET(std::ostream& s, Url& url) {
    eckit::PathName path("~/http/" + url.name());

    eckit::AutoStdFile f(path, "r");
    char line[1024];

    s << HttpStream::dontEncode;

    while (fgets(line, sizeof(line), f)) {
        s << line;
    }

    s << HttpStream::doEncode;
}

static FileResource fileResourceInstance;


}  // namespace eckit
