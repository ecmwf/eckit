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

#include "eckit/maths/Context.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Context::Context(Context* parent): parent_(parent)
{
}

Context::Context(ExpPtr a, Context* parent): parent_(parent)
{
    args_.push_back(a);
}

Context::Context(ExpPtr a, ExpPtr b, Context* parent): parent_(parent)
{
    args_.push_back(a);
    args_.push_back(b);
}

Context::~Context() {
    // Check that all the arguments have been consumed
    ASSERT(!args_.size());
    if(args_.size()) {
        cout << "Context::~Context() leftovers:" << endl;
        for(int i = 0; i < args_.size(); i++ ) {
            cout << args_[i]->str() << endl;
        }
    }
}


ExpPtr Context::nextArg()
{
    ASSERT(args_.size());
    ExpPtr e = args_.front();
    args_.pop_front();
    return e;
}

ExpPtr Context::param(const string& name) const
{
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

void Context::param(const string& name, ExpPtr e)
{
    ASSERT(params_.find(name) == params_.end());
    params_[name] = e;
}

void Context::print( std::ostream& out) const
{
    out << "Context[parent=";
    if(parent_) {
        out << *parent_;
    }
    else {
        out << "null";
    }

    out << ",args=(";

    for(size_t i = 0; i < args_.size(); ++i) {
        if(i) out << ",";
        out << *(args_[i]);
    }

    out << "),params={";

    size_t i = 0;
    for(map<string, ExpPtr>::const_iterator j = params_.begin(); j != params_.end(); ++j, ++i) {
        if(i) out << ",";
        out << (*j).first << ":" << *((*j).second);
    }


    out << "}";

    out << "]";
}
//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
