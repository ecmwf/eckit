/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"

#include "GlobHandler.h"

#include <string.h>
#include <glob.h>

using namespace std;
using namespace eckit;

int glob_error_handler(const char* fn, int errnum)
{
    char buf[1024];
    Log::warning() << "glob: error reading '" << fn << "': " << strerror_r(errnum, buf, sizeof(buf)) << endl;
    return 0;
}

namespace eckit {

GlobHandler::GlobHandler(const std::string& name) : RequestHandler(name) {}

// glob, _ = <pattern> 
Values GlobHandler::handle(ExecutionContext& context)
{
    vector<string> patterns (context.environment().lookupList("_", context));

    glob_t globbuf;
    int flags (0);
    for (size_t i(0); i < patterns.size(); ++i)
    {
        const char* pattern (patterns[i].c_str());

        int rc = glob(pattern, flags, glob_error_handler, &globbuf);
        rc = rc;
        //ASSERT(rc == 0);
        //GLOB_NOSPACE for running out of memory,
        //GLOB_ABORTED for a read error, and
        //GLOB_NOMATCH for no found matches.

        flags = flags | GLOB_APPEND;
    }

    List r;
    for (size_t i(0); i < globbuf.gl_pathc; ++i)
        r.append(string(globbuf.gl_pathv[i]));

    globfree(&globbuf);

    return r;
}

} // namespace eckit
