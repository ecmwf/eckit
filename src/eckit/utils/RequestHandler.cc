/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/Request.h"

#include "RequestHandler.h"
#include "ExecutionContext.h"

using namespace std;
using namespace eckit;

RequestHandler::RequestHandler(const string& name): name_(name) {}

Values RequestHandler::handle(const Request&) { return Values(); }

std::string RequestHandler::name() const { return name_; }

std::string RequestHandler::getValueAsString(const Request& request, const string& keyword, const std::string& defaultValue) const
{
    vector<string> values;
    if (request.find(keyword) != request.end())
        values = request.at(keyword);
    if (values.size() > 1)
        throw UserError(string("Expected only one value of " + keyword)); 
    return values.size() ? values[0] : defaultValue;
}

PathName RequestHandler::getValueAsPathName(const Request& request, const string& keyword, const std::string& defaultValue) const
{ return getValueAsString(request, keyword, defaultValue); }

string RequestHandler::database(const Request& request) 
{
    vector<string> values (request.at("database"));
    if (values.size() != 1)
        throw UserError("You must specify DATABASE explicitly");
    return values[0];
}

vector<string> RequestHandler::pathNamesToStrings(const vector<PathName>& ps)
{
    vector<string> r;
    for (size_t i(0); i < ps.size(); ++i)
        r.push_back(ps[i]);
    return r;
}

void RequestHandler::popIfNotSet(const string& keyword, Request& request, ExecutionContext& context)
{
    vector<string> values (request[keyword]);
    if (! values.size())
    {
        Values vs (context.stack().pop());
        Log::info() << keyword << " not set. Using " << vs.size() << " values from the stack: " << vs << endl;
        request[keyword] = vs;
    }
}

