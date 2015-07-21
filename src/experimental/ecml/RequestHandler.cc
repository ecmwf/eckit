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

#include "parser/Request.h"

#include "RequestHandler.h"
#include "ExecutionContext.h"
#include "Environment.h"
#include "Interpreter.h"

using namespace std;
using namespace eckit;

std::map<std::string,RequestHandler*> RequestHandler::registeredHandlers_ = std::map<std::string,RequestHandler*>();

RequestHandler::RequestHandler(const string& name)
: name_(name)
{
    registeredHandlers_[name] = this;
}

RequestHandler& RequestHandler::handler(const std::string& name)
{
    if (registeredHandlers_.find(name) == registeredHandlers_.end())
        throw UserError(string("RequestHandler ") + name + " not found");

    return *registeredHandlers_[name];
}

std::string RequestHandler::name() const { return name_; }

std::vector<std::string> RequestHandler::getValueAsList(ExecutionContext& context, const std::string& keyword)
{
    std::vector<std::string> r;

    Request v (context.environment().lookup(keyword));
    ASSERT(v->tag() == "_list");

    Request evaluated (context.interpreter().eval(v, context));
    for (Request p(evaluated); p; p = p->rest())
    {
        r.push_back(p->value()->text());
    }

    return r;
}

string RequestHandler::database(ExecutionContext& context) 
{
    string r (context.environment().lookup("database", "", context));
    if (! r.size())
        throw UserError("You must specify DATABASE explicitly");
    return r;
}

string RequestHandler::database(Request request) 
{
    string r (request->valueAsString("database", ""));
    if (! r.size())
        throw UserError("You must specify DATABASE explicitly");
    return r;
}

long RequestHandler::port(ExecutionContext& context) 
{
    string r (context.environment().lookup("port", "9000", context));
    return atol(r.c_str());
}

long RequestHandler::port(Request request)
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

