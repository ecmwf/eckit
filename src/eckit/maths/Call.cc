/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Call.h"
#include "eckit/maths/Lambda.h"
#include "eckit/maths/Scope.h"
#include "eckit/maths/Value.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Call::Call( ExpPtr f) : Function()
{
    args_.push_back(f);
}

Call::Call( ExpPtr f, ExpPtr a) : Function()
{
    args_.push_back(f);
    args_.push_back(a);
}

Call::Call( ExpPtr f, ExpPtr a, ExpPtr b) : Function()
{
    args_.push_back(f);
    args_.push_back(a);
    args_.push_back(b);
}

string Call::returnSignature() const
{
    return ";";
}

ExpPtr Call::evaluate( Scope &ctx ) const
{
    ExpPtr f = param(0, ctx);

    Scope scope("Call::evaluate", &ctx);
    for(size_t i = 1; i < arity(); ++i) {
        scope.pushArg(param(i, ctx)->eval(ctx));
    }

    //std::cout << std::endl << "SCOPE OF CALL is " << scope << std::endl;

    return f->eval(scope)->as<Lambda>()->call(scope);
}

//--------------------------------------------------------------------------------------------

ExpPtr call( ExpPtr f)
{
    return ExpPtr( new Call(f) );
}

ExpPtr call( ExpPtr f, ExpPtr a)
{
    return ExpPtr( new Call(f, a) );
}

ExpPtr call( ExpPtr f, ExpPtr a, ExpPtr b)
{
    return ExpPtr( new Call(f, a, b) );
}


void Call::asCode(std::ostream&o) const
{
    o << "maths::call("; printArgs(o); o << ")";
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
