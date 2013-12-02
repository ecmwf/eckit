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
    push_back(f);
}

Call::Call( ExpPtr f, ExpPtr a) : Function()
{
    push_back(f);
    push_back(a);
}

Call::Call( ExpPtr f, ExpPtr a, ExpPtr b) : Function()
{
    push_back(f);
    push_back(a);
    push_back(b);
}

std::string Call::returnSignature() const
{
    return ";";
}

ExpPtr Call::evaluate( Scope &ctx ) const
{
    ExpPtr f = args(0, ctx, true);

    Scope scope("Call::evaluate", &ctx);
    for(size_t i = 1; i < arity(); ++i) {
        scope.pushArg(args(i, ctx, true));
    }

    //std::cout << std::endl << "SCOPE OF CALL is " << scope << std::endl;

    return f->as<Lambda>()->call(scope);
}

ExpPtr Call::cloneWith(args_t &a) const {
    NOTIMP;
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
