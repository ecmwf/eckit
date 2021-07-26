/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/web/FileResource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/Url.h"


namespace eckit {


FileResource::FileResource() :
    HtmlResource("/files") {}

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
