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
#include "eckit/maths/Context.h"
#include "eckit/maths/ParamDef.h"


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

Function::Function( ExpPtr a, ExpPtr b, ExpPtr body ) : Func()
{
    args_.push_back(a);
    args_.push_back(b);
    args_.push_back(body);
}

Function::Function( ExpPtr a, ExpPtr body ) : Func()
{
    args_.push_back(a);
    args_.push_back(body);
}



string Function::ret_signature() const
{
    return "()";
}

ValPtr Function::evaluate( Context &ctx )
{
    std::cout << "evalute " << *this << " with " << ctx << endl;


    ASSERT(arity());
    size_t last = arity()-1;
    ExpPtr body = param(last, ctx);

    Context scope(&ctx);

    for(size_t i = 0; i < last; ++i) {

        ExpPtr a = param(i);
        ASSERT(ParamDef::is(a));
        const string& name = a->as<ParamDef>()->name();

        ExpPtr b = param(i, ctx);
        scope.param(name, b);
    }

   std::cout << "scope is " << scope << endl;

    return body->eval(scope, true);
}

//--------------------------------------------------------------------------------------------

ExpPtr function( ExpPtr a, ExpPtr b, ExpPtr body )
{
    return ExpPtr( new Function(a, b, body) );
}

ExpPtr function( ExpPtr a, ExpPtr body )
{
    return ExpPtr( new Function(a, body) );
}


ExpPtr function( ExpPtr body )
{
    return ExpPtr( new Function(body) );
}


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
