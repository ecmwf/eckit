/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "GetenvHandler.h"

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"

using namespace std;
using namespace eckit;

namespace eckit {

GetenvHandler::GetenvHandler(const std::string& name) : RequestHandler(name) {}

Values GetenvHandler::handle(ExecutionContext& context)
{
    List r;

    vector<string> vars (context.getValueAsList("values"));
    for (size_t i (0); i < vars.size(); ++i)
    {
        char *s (getenv(vars[i].c_str()));
        r.append(! s ? string("") : string(getenv(vars[i].c_str())));
    }

    return r;
}

} // namespace eckit
