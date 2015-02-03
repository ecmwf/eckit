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

#include "eckit/xpr/Bind.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Boolean.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Scope.h"


namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Bind::Bind( size_t i, ExpPtr f, ExpPtr e ) : Function()
{
    ASSERT( i > 1 );
    push_back( xpr::scalar( (scalar_t)i ) ); // casted to scalar_t (real)
    push_back(f);
    push_back(e);
}

Bind::Bind( ExpPtr i, ExpPtr f, ExpPtr e ) : Function()
{
    push_back(i);
    push_back(f);
    push_back(e);
}

Bind::Bind(args_t& a) : Function(a)
{
    ASSERT( a.size() == 3 );
}

Bind::Bind(Stream &s) : Function(s) {}

void Bind::asCode(std::ostream&o) const
{
    o << factoryName() << "<" << Scalar::extract(args()[0]);
    o << ">(" << *args()[1] << ", " << *args()[2] << ")";
}

std::string Bind::typeName() const
{
    return Bind::nodeName();
}

std::string Bind::returnSignature() const
{
    return args(0)->returnSignature();
}

ExpPtr Bind::evaluate( Scope &ctx ) const
{    
    ExpPtr ix = args(0, ctx, true);

    const size_t i = static_cast<size_t>( Scalar::extract(ctx, ix) );

    ExpPtr f = args(1, ctx, false);
    ExpPtr e = args(2, ctx, true);

    ASSERT(i>=1);
    ctx.insertArg(i-1, e);

    return f->eval(ctx);
}

ExpPtr Bind::cloneWith(args_t& a) const
{
    return ExpPtr( new Bind(a) );
}

//--------------------------------------------------------------------------------------------

ClassSpec Bind::classSpec_ = {
    &Function::classSpec(),
    Bind::nodeName().c_str(),
};

Reanimator< Bind > Bind::reanimator_;

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
