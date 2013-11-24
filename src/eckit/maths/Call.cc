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


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------


Call::Call(const args_t& args) : Func(args)
{
    ASSERT(arity() > 0);
}

Call::Call( ExpPtr f) : Func()
{
    args_.push_back(f);
}


string Call::ret_signature() const
{
    return ";";
}

ValPtr Call::evaluate( Scope &ctx )
{
    ExpPtr f = param(0, ctx);

    args_t args;

    Scope scope("Call::evaluate", &ctx);

    //ctx.tranferArgs(scope);

    //ASSERT(Function::is(f));
    return f->as<Lambda>()->call(scope);
}

//--------------------------------------------------------------------------------------------

ExpPtr call( ExpPtr f)
{
    return ExpPtr( new Call(f) );
}


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
