/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/maths/Bind.h"
#include "eckit/maths/List.h"
#include "eckit/maths/Boolean.h"
#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Bind::Bind(const args_t& args) : Function(args)
{
    ASSERT(arity() == 2);
}

Bind::Bind( size_t i, ExpPtr f, ExpPtr e ) : Function()
{
    ASSERT( i > 0 );
    ASSERT( i <= f->arity() );

    args_.push_back( maths::scalar( (scalar_t)i ) ); // casted to scalar_t (real)
    args_.push_back(f);
    args_.push_back(e);
}

Bind::Bind( ExpPtr i, ExpPtr f, ExpPtr e ) : Function()
{
    args_.push_back(i);
    args_.push_back(f);
    args_.push_back(e);
}

string Bind::typeName() const
{
    return Bind::className();
}

string Bind::returnSignature() const
{
    return args_[0]->returnSignature();
}

ExpPtr Bind::evaluate( Scope &ctx )
{    
    DBGX(*this);

    ExpPtr ix = param(0,ctx);

    DBGX(*ix);

    const size_t i = static_cast<size_t>( Scalar::extract(ix) );

    ExpPtr f = param(1,ctx);

    DBGX(*f);

    ASSERT( i > 0 );
    ASSERT( i <= f->arity() );

    ExpPtr e = param(2,ctx);

    DBGX(*e);

    //FIXME: This should not be called
    ExpPtr fx = f->clone();

    DBGX(*fx);

    fx->param( (i - 1), e );

    DBGX(*fx);

    return fx->evaluate(ctx);
}


void Bind::asCode(ostream &o) const
{
    o << "maths::bind("; printArgs(o); o << ")";
}


//--------------------------------------------------------------------------------------------

static Function::RegisterFactory< Bind > bind_register;                                                  \

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
