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

//static Func::RegisterFactory< Lambda > fmap_register;

Lambda::Lambda(const args_t& args) : Func(args)
{
}

Lambda::Lambda( ExpPtr body ) : Func()
{
    args_.push_back(body);
}

Lambda::Lambda( const string& a, const string& b, ExpPtr body ) : Func()
{
    args_.push_back(maths::paramdef(a));
    args_.push_back(maths::paramdef(b));
    args_.push_back(body);
}

Lambda::Lambda( const string &a, ExpPtr body ) : Func()
{
    args_.push_back(maths::paramdef(a));
    args_.push_back(body);
}



string Lambda::returnSignature() const
{
    return "()";
}

ValPtr Lambda::evaluate( Scope &ctx )
{
    return boost::static_pointer_cast<Value>( shared_from_this() );
}

ValPtr Lambda::call( Scope &ctx )
{

    //std::cout << endl << "EVALUATE LAMBDA " << *this << endl << " WITH " << ctx << endl;
    //std::cout << "LAMBDA ARITY " << arity() << endl;

    ASSERT(arity());
    size_t last = arity()-1;
    ExpPtr body = param(last, ctx);

   // std::cout << "LAMBDA BODY IS " << *body << endl;

    Scope scope("Lambda::evaluate",&ctx);

    for(size_t i = 0; i < last; ++i) {

        ExpPtr a = param(i);
        //cout << "LAMBDA ARG " << i << " IS " << *a << endl;
        ASSERT(ParamDef::is(a));
        const string& name = a->as<ParamDef>()->name();
        //cout << "LAMBDA NAME " << i << " IS " << name << endl;

        ExpPtr b = param(i, ctx);
        //cout << "LAMBDA " << name << " IS " << *b << endl;
        scope.param(name, b);
    }

    //std::cout << endl << "SCOPE OF LAMBDA IS " << scope << endl;

    return body->eval(scope, true);
}


void Lambda::asCode(ostream &o) const
{
    o << "maths::lambda(" << printArgs(o) << ")";
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
