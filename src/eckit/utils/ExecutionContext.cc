/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"
#include "eckit/utils/Module.h"

using namespace std;

ExecutionContext::ExecutionContext()
: stack_(),
  environment_(new Environment(0))
{}

ExecutionContext::~ExecutionContext()
{
    for (Environment* e(environment_); e; e = e->parent())
        delete e;
}

Stack& ExecutionContext::stack() { return stack_; }

Environment& ExecutionContext::environment() { return *environment_; }

void ExecutionContext::import(Module& module)
{
    pushEnvironmentFrame();
    module.importInto(*this);
}

void ExecutionContext::pushEnvironmentFrame()
{
    environment_ = new Environment(environment_);
}

void ExecutionContext::popEnvironmentFrame()
{
    ASSERT(environment_ && environment_->parent());

    Environment *e (environment_);
    environment_ = e->parent();
    delete e;
}


