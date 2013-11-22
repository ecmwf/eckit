/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Function.h"
#include "eckit/maths/Boolean.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

//static Func::RegisterFactory< Function > fmap_register;

Function::Function(const args_t& args) : Func(args)
{
}

Function::Function( ExpPtr body ) : Func()
{
    args_.push_back(body);
}

Function::Function( ExpPtr arg, ExpPtr body ) : Func()
{
    args_.push_back(arg);
    args_.push_back(body);
}

Function::Function( ExpPtr arg1, ExpPtr arg2, ExpPtr body ) : Func()
{
    args_.push_back(arg1);
    args_.push_back(arg2);
    args_.push_back(body);
}

string Function::ret_signature() const
{
    return "?";
}

ValPtr Function::evaluate( context_t& ctx )
{
    ASSERT(arity());

    ExpPtr body = param(arity()-1, &ctx);

    args_t args;
    for(size_t i = 0; i < arity()-1; ++i) {
        args.push_back(param(i, &ctx));
    }

    return body->eval(args);


}

//--------------------------------------------------------------------------------------------

ExpPtr function( ExpPtr body )
{
    return ExpPtr( new Function(body) );
}

ExpPtr function( ExpPtr arg,  ExpPtr body )
{
    return ExpPtr( new Function(arg, body) );
}

ExpPtr function( ExpPtr arg1,  ExpPtr arg2, ExpPtr body )
{
    return ExpPtr( new Function(arg1, arg2, body) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
