/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Take.h"
#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------
Take::Take(ExpPtr e, ExpPtr l) : Function()
{
    args_.push_back(e);
    args_.push_back(l);
}

std::string Take::returnSignature() const
{
    return Undef::sig();
}

ExpPtr Take::evaluate( Scope &ctx ) const
{
    ExpPtr idx  = param(0, ctx)->eval(ctx);
    ExpPtr list = param(1, ctx)->eval(ctx);

    size_t i = (size_t) Scalar::extract( ctx, idx );

    if( i >= list->arity() )
        throw UserError("Take supplied index larger than size of list" );

    return list->param(i);
}


void Take::asCode(std::ostream&o) const
{
    o << "maths::take("; printArgs(o); o << ")";
}

ExpPtr Take::cloneWith(args_t &a) const {
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ExpPtr take( ExpPtr e, ExpPtr l )
{
    return ExpPtr( new Take(e,l) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
