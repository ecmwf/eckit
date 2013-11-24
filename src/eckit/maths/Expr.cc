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

#include "eckit/maths/Expr.h"
#include "eckit/maths/Value.h"
#include "eckit/maths/Scope.h"
#include "eckit/maths/Undef.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/ParamDef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Expr::Expr()
{
}

Expr::Expr(const args_t &args) :
    args_(args)
{
}

Expr::~Expr()
{
}


ValPtr Expr::eval()
{
    Scope ctx("Expr::eval()");
    return optimise()->evaluate(ctx);
}

ValPtr Expr::eval( Scope& ctx, bool)
{
    return optimise()->evaluate(ctx);
}


ValPtr Expr::eval(Scope& scope, const args_t& args)
{
    Scope ctx("Expr::eval(Scope& scope, const args_t& args)", &scope);
    for(args_t::const_iterator j = args.begin(); j != args.end(); ++j) {
        ctx.pushArg(*j);
    }
    return optimise()->evaluate(ctx);
}

ExpPtr Expr::resolve(Scope & ctx)
{
    return self();
}

ValPtr Expr::eval( ExpPtr e )
{
    Scope ctx("Expr::eval( ExpPtr e )", e);
    ValPtr res = optimise()->evaluate(ctx);
    return res;
}

ValPtr Expr::eval(  ExpPtr a, ExpPtr b )
{
    Scope ctx("Expr::eval(  ExpPtr a, ExpPtr b )", a, b);
    ValPtr res = optimise()->evaluate(ctx);
    return res;
}

ExpPtr Expr::param( size_t i) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );
    return args_[i];
}


ExpPtr Expr::param( size_t i, Scope& ctx ) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );

    //cout << "   PARAM " << i << " -> " << (*args_[i]) << endl;
    //cout << "         " << ctx << endl;
    ExpPtr r = args_[i]->resolve(ctx);
    //cout << "          " << *r << endl;

    return r;
}

void Expr::param(size_t i, ExpPtr p)
{
    ASSERT( i < args_.size() );
    ASSERT( p );
    if( p != args_[i] )
        args_[i] = p;
}

string Expr::str() const
{
    std::ostringstream os;
    print(os);
    return os.str();
}


ostream& Expr::printArgs(ostream &out) const
{
    size_t count = arity();
    for(size_t i = 0; i < count; ++i) {
        if(i) out << ", ";
        out << *args_[i];
    }
    return out;
}

std::ostream& operator<<( std::ostream& os, const Expr& v)
{
    if(format(os) == maths::CodeFormat)
    {
        v.asCode(os);
    }
    else {
        v.print(os);
    }
    return os;
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
