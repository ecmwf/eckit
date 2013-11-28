/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Lambda.h"
#include "eckit/maths/Boolean.h"
#include "eckit/maths/Scope.h"
#include "eckit/maths/ParamDef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Lambda::Lambda( ExpPtr body ) : Function()
{
    push_back(body);
}

Lambda::Lambda( const string& a, const string& b, ExpPtr body ) : Function()
{
    push_back(maths::paramdef(a));
    push_back(maths::paramdef(b));
    push_back(body);
}

Lambda::Lambda( const string &a, ExpPtr body ) : Function()
{
    push_back(maths::paramdef(a));
    push_back(body);
}


string Lambda::returnSignature() const
{
    return "()";
}

ExpPtr Lambda::evaluate( Scope &ctx ) const
{
    return self();
}

ExpPtr Lambda::call( Scope &ctx ) const
{

    //std::cout << std::endl << "EVALUATE LAMBDA " << *this << std::endl << " WITH " << ctx << std::endl;
    //std::cout << "LAMBDA ARITY " << arity() << std::endl;

    ASSERT(arity());
    size_t last = arity()-1;
    ExpPtr body = param(last, ctx);

   // std::cout << "LAMBDA BODY IS " << *body << std::endl;

    Scope scope("Lambda::evaluate",&ctx);

    for(size_t i = 0; i < last; ++i) {

        ExpPtr a = param(i);
        //std::cout << "LAMBDA ARG " << i << " IS " << *a << std::endl;
        ASSERT(ParamDef::is(a));
        const string& name = a->as<ParamDef>()->name();
        //std::cout << "LAMBDA NAME " << i << " IS " << name << std::endl;

        ExpPtr b = param(i, ctx);
        //std::cout << "LAMBDA " << name << " IS " << *b << std::endl;
        scope.param(name, b);
    }

    //std::cout << std::endl << "SCOPE OF LAMBDA IS " << scope << std::endl;

    return body->eval(scope);
}


void Lambda::asCode(std::ostream&o) const
{
    o << "maths::lambda("; printArgs(o); o <<")";
}

ExpPtr Lambda::cloneWith(args_t &a) const {
    NOTIMP;
}

//--------------------------------------------------------------------------------------------

ExpPtr lambda( const string &a, const string& b, ExpPtr body )
{
    return ExpPtr( new Lambda(a, b, body) );
}

ExpPtr lambda( const string &a, ExpPtr body )
{
    return ExpPtr( new Lambda(a, body) );
}


ExpPtr lambda( ExpPtr body )
{
    return ExpPtr( new Lambda(body) );
}


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
