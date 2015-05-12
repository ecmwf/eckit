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

#include "eckit/utils/RequestHandler.h"
#include "eckit/utils/Environment.h"

using namespace std;
using namespace eckit;

Environment::Environment(Environment* parent)
: parent_(parent)
{}

Environment::~Environment()
{
    for (map<string,RequestHandler*>::iterator it(dictionary_.begin()); it != dictionary_.end(); ++it)
        delete it->second;
    dictionary_.clear();
}

void Environment::print(std::ostream& s) const
{
    s << "{";
    for (map<string,RequestHandler*>::const_iterator it(dictionary_.begin()); it != dictionary_.end(); ++it)
        s << it->first << ",";
    s << "}";
}

void Environment::set(const std::string& name, RequestHandler* handler)
{
    dictionary_[name] = handler;
}

RequestHandler* Environment::lookup(const std::string& name)
{
    map<string,RequestHandler*>::iterator it (dictionary_.find(name));
    if (it != dictionary_.end())
        return it->second;

    if (parent_)
        return parent_->lookup(name);

    throw UserError (name + " not defined");
}


Environment* Environment::parent() { return parent_; }
