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

Request RequestHandler::getValue(const Request request, const string& keyword, const Request defaultValue) 
{
    //request->showGraph(string("getValue: request = ") + request->str() + ", keyword = " + keyword);
    std::string k (StringTools::lower(keyword));
    Request p(0);
    for (Request r (request); r; r = r->rest())
        if (StringTools::lower(r->name()) == k)
            p = r;

    if (p == 0)
        return defaultValue;

    return p;
}

std::vector<std::string> RequestHandler::getValueAsList(const Request request, const std::string& keyword)
{
    //request->showGraph(string("getValueAsList: request = ") + request->str() + ", keyword = " + keyword);

    std::vector<std::string> r;

    Request v(getValue(request, keyword, 0));
    if (! v)
        throw UserError(string("Keyword '") + keyword + "' not found");

    ASSERT(v->value() && v->value()->name() == "_list");

    for (Request p(v->value()); p; p = p->rest())
        r.push_back(p->value()->name());

    Log::info() << "getValueAsList(request=" << request <<  ", keyword=" << keyword << ") => " << r << endl;

    return r;
}

std::string RequestHandler::getValueAsString(const Request request, const string& keyword, const std::string& defaultValue)
{
    //request->showGraph(string("getValueAsString: request = ") + request->str() + ", keyword = " + keyword);

    Request p(getValue(request, keyword, 0));

    //request->showGraph(string("getValueAsString: found ") + p->str());

    if (! p)
        return defaultValue;

    ASSERT(p->value()->name() == "_list");
    string r (p->value()->value()->name());

    if (p->value()->rest())
        throw UserError(string("Expected only one value of ") + keyword + ", found: " + p->value()->str());

    return r;
}

PathName RequestHandler::getValueAsPathName(const Request request, const string& keyword, const std::string& defaultValue)
{
    return getValueAsString(request, keyword, defaultValue);
}

string RequestHandler::database(const Request request) 
{
    //request->showGraph(string("database: request = ") + request->str());

    string r (getValueAsString(request, "database", ""));
    if (r == "")
        throw UserError("You must specify DATABASE explicitly");
    
    Log::info() << "database => " << r << endl;

    return r;
}

long RequestHandler::port(const Request request) 
{
    string r (getValueAsString(request, "port", "9000"));
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
    Request v(getValue(request, keyword, 0));
    if (! v)
    {
        //Values vs (context.stack().pop());
        v = context.stack().pop();
        Log::info() << keyword << " not set. Using values from the stack: " << v << endl;
        //request[keyword] = vs;
        setValue(request, keyword, v);
    }
}

Request RequestHandler::setValue(const Request request, const std::string& keyword, const std::string& value)
{
   return setValue(request, keyword, new Cell("_list", new Cell(value, 0, 0), 0));
}

Request RequestHandler::setValue(const Request request, const std::string& keyword, const Request v) 
{
    //request->showGraph(string("setValue: request = ") + request->str() + ", keyword = " + keyword + ", v = " + v->str());

    for (Request r (request); r; r = r->rest())
    {
        if (r->name() == keyword)
            return r->value(v);

        if (! r->rest())
            return r->rest(new Cell(keyword, v, 0))->value();
    }

    ASSERT("Should not reach here" && 0);
    return 0;
}
