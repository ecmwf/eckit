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
#include "SpecialFormHandler.h"
#include "ExecutionContext.h"
#include "Environment.h"
#include "Interpreter.h"

using namespace std;
using namespace eckit;

std::map<std::string,SpecialFormHandler*> SpecialFormHandler::registeredHandlers_ = std::map<std::string,SpecialFormHandler*>();

SpecialFormHandler::SpecialFormHandler(const string& name)
: name_(name)
{
    registeredHandlers_[name] = this;
}

SpecialFormHandler& SpecialFormHandler::handler(const std::string& name)
{
    if (registeredHandlers_.find(name) == registeredHandlers_.end())
        throw UserError(string("SpecialFormHandler ") + name + " not found");

    return *registeredHandlers_[name];
}

std::string SpecialFormHandler::name() const { return name_; }

