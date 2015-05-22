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
#include "eckit/parser/StringTools.h"
#include "eckit/parser/Request.h"

#include "RequestHandler.h"
#include "ExecutionContext.h"

using namespace std;
using namespace eckit;

RequestHandler::RequestHandler(const string& name): name_(name) {}

Values RequestHandler::handle(const Request) { return Values(); }

std::string RequestHandler::name() const { return name_; }

std::vector<std::string> RequestHandler::getValueAsList(const Request request, const std::string& keyword)
{
    //request->showGraph(string("getValueAsList: request = ") + request->str() + ", keyword = " + keyword);

    std::vector<std::string> r;

    Request v (request->valueOrDefault(keyword, 0));
    if (! v)
        throw UserError(string("Keyword '") + keyword + "' not found");

    ASSERT(v->value() && v->value()->name() == "_list");

    for (Request p(v->value()); p; p = p->rest())
        r.push_back(p->value()->name());

    Log::info() << "getValueAsList(request=" << request <<  ", keyword=" << keyword << ") => " << r << endl;

    return r;
}

string RequestHandler::database(const Request request) 
{
    //request->showGraph(string("database: request = ") + request->str());

    string r (request->valueAsString("database", ""));
    if (! r.size())
        throw UserError("You must specify DATABASE explicitly");
    
    Log::info() << "database => " << r << endl;

    return r;
}

long RequestHandler::port(const Request request) 
{
    string r (request->valueAsString("port", "9000"));
    return atol(r.c_str());
}

vector<string> RequestHandler::pathNamesToStrings(const vector<PathName>& ps)
{
    vector<string> r;
    for (size_t i(0); i < ps.size(); ++i)
        r.push_back(ps[i]);
    return r;
}

void RequestHandler::popIfNotSet(const string& keyword, Request request, ExecutionContext& context)
{
    //request->showGraph("popIfNotSet: '" + keyword + "'");
    if (! request->valueOrDefault(keyword, 0))
    {
        Request v = context.stack().pop();

        Log::info() << keyword << " not set. Using values from the stack: " << v << endl;

        request->value(keyword, v);
    }
}

