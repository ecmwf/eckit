/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Call.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/Scope.h"
#include "eckit/xpr/Value.h"


namespace eckit {
namespace xpr {

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

Call::Call(args_t& a) : Function(a)
{
    ASSERT( a.size() >= 1 && a.size() <= 3);
}

Call::Call(Stream &s) : Function(s) {}

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

ExpPtr Call::cloneWith( args_t& a ) const
{
    return ExpPtr( new Call(a) );
}

void Call::encode(Stream &s) const
{
    Function::encode(s);
}

//--------------------------------------------------------------------------------------------

ClassSpec Call::classSpec_ = {
    &Function::classSpec(),
    Call::nodeName().c_str(),
};

Reanimator< Call > Call::reanimator_;

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

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
