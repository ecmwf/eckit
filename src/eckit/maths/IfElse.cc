/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/IfElse.h"
#include "eckit/maths/Boolean.h"
#include "eckit/maths/Scope.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

//static Func::RegisterFactory< IfElse > fmap_register;

IfElse::IfElse(const args_t& args) : Func(args)
{
    ASSERT(arity() == 3);
}

IfElse::IfElse( ExpPtr f,  ExpPtr i, ExpPtr e ) : Func()
{
    args_.push_back(f);
    args_.push_back(i);
    args_.push_back(e);

    ASSERT(arity() == 3);
}

string IfElse::returnSignature() const
{
    string i = param(1)->returnSignature();
    string e = param(2)->returnSignature();

    if(i == e) {
        return i;
    }
    else {
        return "?";
    }
}

ValPtr IfElse::evaluate( Scope &ctx )
{
    //std::cout << "evalute " << *this << " with " << ctx << endl;

    ExpPtr f = param(0, ctx);
    ExpPtr i = param(1, ctx);
    ExpPtr e = param(2, ctx);

    ExpPtr v = f->evaluate(ctx);

    bool b = Boolean::extract( v );

    if(b) {
        return i->eval(ctx, true);
    }
    else {
        return e->eval(ctx, true);
    }


}


void IfElse::asCode(ostream &o) const
{
    o << "maths::ifelse(" << printArgs(o) << ")";
}


//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f,  ExpPtr i, ExpPtr e )
{
    return ExpPtr( new IfElse(f, i, e) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
