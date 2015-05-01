/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/Boolean.h"
#include "eckit/xpr/Scope.h"
#include "eckit/xpr/ParamDef.h"


namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Lambda::Lambda( ExpPtr body ) : Function()
{
    push_back(body);
}

Lambda::Lambda( const std::string& a, const std::string& b, ExpPtr body ) : Function()
{
    push_back(xpr::paramdef(a));
    push_back(xpr::paramdef(b));
    push_back(body);
}

Lambda::Lambda( const std::string &a, ExpPtr body ) : Function()
{
    push_back(xpr::paramdef(a));
    push_back(body);
}

Lambda::Lambda(args_t& a) : Function(a)
{
    ASSERT( a.size() <= 3 );
}

Lambda::Lambda(Stream &s) : Function(s) {}

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
    ExpPtr body = args(last, ctx, false);

   // std::cout << "LAMBDA BODY IS " << *body << std::endl;

    Scope scope("Lambda::evaluate",&ctx);

    for(size_t i = 0; i < last; ++i) {

        ExpPtr a = args(i);
        //std::cout << "LAMBDA ARG " << i << " IS " << *a << std::endl;
        ASSERT(ParamDef::is(a));
        const std::string& name = a->as<ParamDef>()->name();
        //std::cout << "LAMBDA NAME " << i << " IS " << name << std::endl;

        ExpPtr b = args(i, ctx, true);
        //std::cout << "LAMBDA " << name << " IS " << *b << std::endl;
        scope.param(name, b);
    }

    //std::cout << std::endl << "SCOPE OF LAMBDA IS " << scope << std::endl;

    return body->eval(scope);
}

ExpPtr Lambda::cloneWith(args_t& a) const
{
    return ExpPtr(new Lambda(a));
}

//--------------------------------------------------------------------------------------------

ClassSpec Lambda::classSpec_ = {
    &Function::classSpec(),
    Lambda::nodeName(),
};

Reanimator< Lambda > Lambda::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr lambda( const std::string& a, const std::string& b, ExpPtr body )
{
    return ExpPtr( new Lambda(a, b, body) );
}

ExpPtr lambda( const std::string &a, ExpPtr body )
{
    return ExpPtr( new Lambda(a, body) );
}

ExpPtr lambda( ExpPtr body )
{
    return ExpPtr( new Lambda(body) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
