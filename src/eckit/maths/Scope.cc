/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/maths/Scope.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Scope::Scope(const char* name, Scope* parent): parent_(parent), name_(name)
{
}

Scope::Scope(const char* name, ExpPtr a, Scope* parent): parent_(parent), name_(name)
{
    args_.push_back(a);
}

Scope::Scope(const char* name, ExpPtr a, ExpPtr b, Scope* parent): parent_(parent), name_(name)
{
    args_.push_back(a);
    args_.push_back(b);
}

Scope::~Scope() {

    if(!Exception::throwing()) {
        // Check that all the arguments have been consumed

        if(args_.size()) {
            cout << "Context::~Context() leftovers: " << *this << endl;
        }

       ASSERT(!args_.size());
    }
}

ExpPtr Scope::nextArg()
{

    if(args_.size() == 0) {
        if(parent_) {
            return parent_->nextArg();
        }
    }

    ASSERT(args_.size());
    ExpPtr e = args_.front();
    args_.pop_front();
    return e;
}

void Scope::pushArg(ExpPtr e)
{
    args_.push_back(e);
}

ExpPtr Scope::param(const string& name) const
{
    //cout << endl << "LOOKING FOR '" << name << "' in " << *this << endl;
    map<string, ExpPtr>::const_iterator j = params_.find(name);
    if(j == params_.end()) {
        if(parent_) {
            return parent_->param(name);
        }
        cout << *this << endl;
        throw UserError(string("param not found: " + name));
    }
    return (*j).second;
}

void Scope::param(const string& name, ExpPtr e)
{
    ASSERT(params_.find(name) == params_.end());
    params_[name] = e;
}

void Scope::print( std::ostream& out) const
{
    out << "Scope[name=" << name_;

    out << ", args=(";

    for(size_t i = 0; i < args_.size(); ++i) {
        if(i) out << ",";
        out << *(args_[i]);
    }

    out << "), params={";

    size_t i = 0;
    for(map<string, ExpPtr>::const_iterator j = params_.begin(); j != params_.end(); ++j, ++i) {
        if(i) out << ",";
        out << (*j).first << ":" << *((*j).second);
    }


    out << "}";

    out << ", parent=";
    if(parent_) {
        out << *parent_;
    }
    else {
        out << "null";
    }


    out << "]";
}
//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
