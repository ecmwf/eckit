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

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"

#include <vector>

using namespace std;

namespace eckit {

RangeHandler::RangeHandler(const std::string& name) : RequestHandler(name) {}

Values RangeHandler::handle(ExecutionContext& context)
{
    const string from (context.environment().lookup("from", "", context));
    const string to (context.environment().lookup("to", "", context));
    const string below (context.environment().lookup("below", "", context));

    if (! from.size() || (to.size() == 0 && below.size() == 0) 
                      || (to.size() != 0 && below.size() != 0)) 
        throw UserError("range: one value of 'from' and one value of 'to' or 'below' must be given");

    long lf (atol(from.c_str()));
    long lt (to.size() 
             ? atol(to.c_str())
             : atol(below.c_str()) - 1);

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
