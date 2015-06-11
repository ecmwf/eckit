/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Environment.cc
// Piotr Kuchta - (c) ECMWF May 2015

#include <unistd.h>
#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Environment.h"
#include "eckit/utils/Interpreter.h"
#include "eckit/utils/ExecutionContext.h"

using namespace std;
using namespace eckit;  // TODO: eckit/utils/Environment.h should be in namespace eckit I think

Environment::Environment(Environment* parent, Request r)
: dictionary_(r),
  parent_(parent)
{}

Environment::~Environment()
{
    // TODO
    //for (map<string,Request>::iterator it(dictionary_.begin()); it != dictionary_.end(); ++it)
    //    delete it->second;
    //dictionary_.clear();
}

void Environment::print(std::ostream& s) const
{
    // TODO
    s << "{" << "Environment@" << this << "}";
}

void Environment::set(const std::string& name, Request request)
{
    //dictionary_[name] = request;
    dictionary_->value(name, request);
}

string Environment::lookup(const string& name, const string& defaultValue, ExecutionContext& context)
{
    Request r (lookupNoThrow(name));
    if (! r)
        return defaultValue;

    ASSERT(r->tag() == "_list");
    Request evaluated (context.interpreter().eval(r, context));
    ASSERT(evaluated->tag() == "_list");
    if (evaluated->rest())
        throw UserError("Expected single string as value of " + name);
    return evaluated->value()->text();
}

vector<string> Environment::lookupList(const string& name, ExecutionContext& context)
{
    Request r (context.interpreter().eval(lookup(name), context));

    vector<string> list;
    for (Request e (r); e; e = e->rest())
    {
        ASSERT(r->tag() == "_list");
        ASSERT(r->value()->tag() == "");

        list.push_back(r->value()->text());
    }
    return list;
}

Request Environment::lookupNoThrow(const string& name)
{
    Request r (dictionary_->valueOrDefault(name, 0));
    if (r)
        return r;

    if (! parent_)
        return 0;

    return parent_->lookupNoThrow(name);
}

Request Environment::lookup(const string& name)
{
    Request r(lookupNoThrow(name));
    if (! r)
        throw UserError (name + " not defined");
    return r;
}


Environment* Environment::parent() { return parent_; }

Request Environment::currentFrame() { return dictionary_; }

