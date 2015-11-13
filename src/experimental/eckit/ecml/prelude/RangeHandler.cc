/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "RangeHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"

#include <vector>

using namespace std;

namespace eckit {

RangeHandler::RangeHandler(const std::string& name) : RequestHandler(name) {}

Values RangeHandler::handle(ExecutionContext& context)
{
    vector<string> from (context.environment().lookupList("from", context));
    vector<string> to (context.environment().lookupList("to", context));

    if (from.size() != 1 || to.size() != 1)
        throw UserError("range: one value of 'from' and one value of 'to' must be given");

    long lf (atol(from[0].c_str()));
    long lt (atol(to[0].c_str()));

    List r;

    for (long i (lf); i <= lt; ++i)
    {
        stringstream ss;
        ss << i;
        r.append(ss.str());
    }

    return r;
}

} // namespace eckit
