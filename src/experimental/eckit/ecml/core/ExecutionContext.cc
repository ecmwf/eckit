/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"

#include "ExecutionContext.h"
#include "Environment.h"
#include "Module.h"
#include "Interpreter.h"
#include "RequestHandler.h"
#include "eckit/ecml/parser/RequestParser.h"
#include "eckit/ecml/prelude/Prelude.h"

using namespace std;
using namespace eckit;

ExecutionContext::ExecutionContext()
: environment_(new Environment(0, new Cell("_list", "", 0, 0))),
  otherEnvironment_(0),
  interpreter_(new Interpreter())
{
    Prelude().importInto(*this);
}

ExecutionContext::ExecutionContext(const ExecutionContext& other)
: environment_(other.environment_),
  otherEnvironment_(other.environment_),
  interpreter_(new Interpreter())
{
}

ExecutionContext::~ExecutionContext()
{
    for (Environment* e(environment_); e && e != otherEnvironment_; e = e->parent())
        delete e;
    delete interpreter_;
}

Environment& ExecutionContext::environment() { return *environment_; }

void ExecutionContext::registerHandler(const char* name, eckit::RequestHandler& handler)
{
    registerHandler(string(name), handler);
}

void ExecutionContext::registerHandler(const std::string& name, eckit::RequestHandler& handler)
{
    environment().set(name, new Cell("_native", handler.name(), 0, 0));

    // TODO: save pointer to handler in a map in the context so we don't have to protect lookup operation.
    // ExecutionContext will not be shared between threads.
}

Values ExecutionContext::executeScriptFile(const string& fileName)
{
    return interpreter().eval(RequestParser::parseFile(fileName.c_str(), /*debug=*/ false), *this);
}

Values ExecutionContext::execute(const string& source)
{
    return interpreter().eval(RequestParser::parse(source, /*debug=*/ false), *this);
}

void ExecutionContext::import(Module& module)
{
    pushEnvironmentFrame();
    module.importInto(*this);
}

void ExecutionContext::pushEnvironmentFrame(Request r)
{
    environment_ = new Environment(environment_, r);
}

void ExecutionContext::pushEnvironmentFrame()
{
    pushEnvironmentFrame(new Cell("_list", "", 0, 0));
}

void ExecutionContext::popEnvironmentFrame()
{
    ASSERT(environment_ && environment_->parent());

    Environment *e (environment_);
    environment_ = e->parent();
    delete e;
}


void ExecutionContext::popEnvironmentFrame(Request r)
{
    while (environment_ && environment_->currentFrame() != r)
        popEnvironmentFrame(); 
    ASSERT(environment_);
    popEnvironmentFrame(); 
}

void ExecutionContext::interpreter(Interpreter* interpreter)
{
    delete interpreter_;
    interpreter_ = interpreter;
}

Interpreter& ExecutionContext::interpreter() const
{ 
    return *interpreter_;
}

Cell* ExecutionContext::copy() const
{
    // TODO:
    NOTIMP;
    return 0;
}

std::vector<std::string> ExecutionContext::getValueAsList(const std::string& keyword)
{
    std::vector<std::string> r;

    Request v (environment().lookup(keyword));
    ASSERT(v->tag() == "_list");

    Request evaluated (interpreter().eval(v, *this));
    for (Request p(evaluated); p; p = p->rest())
        if (p->value())
            r.push_back(p->value()->text());

    return r;
}
