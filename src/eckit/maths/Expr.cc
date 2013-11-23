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
#include "eckit/maths/Context.h"
#include "eckit/maths/Undef.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/ParamDef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Expr::Expr()
{}

Expr::Expr(const args_t &args) :
    args_(args)
{}

Expr::~Expr()
{}


ValPtr Expr::eval()
{
    Context ctx;
    return optimise()->evaluate(ctx);
}

ValPtr Expr::eval( Context& ctx, bool)
{
    return optimise()->evaluate(ctx);
}


ValPtr Expr::eval(Context& scope, const args_t& args)
{
    Context ctx(&scope);
    for(args_t::const_iterator j = args.begin(); j != args.end(); ++j) {
        ctx.pushArg(*j);
    }
    return optimise()->evaluate(ctx);
}


ValPtr Expr::eval( ExpPtr e )
{
    Context ctx(e);
    ValPtr res = optimise()->evaluate(ctx);
    return res;
}

ValPtr Expr::eval(  ExpPtr a, ExpPtr b )
{
    Context ctx(a, b);
    ValPtr res = optimise()->evaluate(ctx);
    return res;
}

ExpPtr Expr::param( size_t i) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );
    return args_[i];
}

ExpPtr Expr::param( size_t i, Context& ctx ) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );

    cout << "   PARAM " << i << " -> " << (*args_[i]) << endl;

    if(Undef::is(args_[i]) )
    {
        return ctx.nextArg();
    }

    if(ParamDef::is(args_[i]) )
    {
        return ctx.nextArg();
    }

    if(Param::is(args_[i]) ) {

        const string& name = args_[i]->as<Param>()->name();
        cout << "    param '" << name << "' " << *(ctx.param(name)) << endl;

        return ctx.param(name);
    }

    return args_[i];
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


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
