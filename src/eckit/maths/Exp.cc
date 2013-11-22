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

#include "eckit/maths/Exp.h"
#include "eckit/maths/Value.h"

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
    context_t ctx;
    cout << "Eval 0 " << ctx.size() << endl;
    return optimise()->evaluate(ctx);
}

ValPtr Expr::eval( ExpPtr e )
{
    context_t ctx;
    ctx.push_back(e);
    cout << "Eval 1 " << ctx.size() << endl;
    ValPtr res = optimise()->evaluate(ctx);
    ASSERT( ctx.empty() );
    return res;
}

ValPtr Expr::eval(  ExpPtr a, ExpPtr b )
{
    context_t ctx;
    ctx.push_back(a);
    ctx.push_back(b);
    cout << "Eval 2 " << ctx.size() << endl;
    ValPtr res = optimise()->evaluate(ctx);
    ASSERT( ctx.empty() );
    return res;
}

ValPtr Expr::eval( const args_t& args) {
    context_t ctx;
    std::copy(args.begin(), args.end(), std::back_inserter(ctx));
    cout << "Eval 3 " << ctx.size() << endl;
    ValPtr res = optimise()->evaluate(ctx);
    ASSERT( ctx.empty() );
    return res;
}


ExpPtr Expr::param( const size_t& i, context_t* ctx ) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );

    if( ctx && Undef::is(args_[i]) )
    {
        ASSERT( !ctx->empty() );
        ExpPtr e = ctx->front();
        ctx->pop_front();
        return e;
    }

     if( ctx && Param::is(args_[i]) ) {
         cout << "Here" << endl;
         size_t n = args_[i]->as<Param>()->n()-1;
         cout << n << endl;
         cout << ctx->size();
         ASSERT( n < ctx->size() );
         return (*ctx)[n];
     }

    return args_[i];
}

void Expr::param(const size_t &i, ExpPtr p)
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

Undef::Undef() : Expr()
{
}

Undef::~Undef()
{
}

ValPtr Undef::evaluate( context_t& ctx )
{
    NOTIMP;
    return boost::static_pointer_cast<Value>( shared_from_this() );
}

ExpPtr Undef::clone()
{
    return undef();
}

ExpPtr undef()
{
    return ExpPtr( new Undef() );
}
//--------------------------------------------------------------------------------------------

Param::Param(size_t n) : Expr(), n_(n)
{
}

Param::~Param()
{
}

ExpPtr Param::clone()
{
    return parameter(n_);
}

void Param::print(ostream &o) const
{
    o << "_(" << n_ << ")";
}

ExpPtr parameter(size_t n)
{
    return ExpPtr( new Param(n) );
}

ValPtr Param::evaluate( context_t& ctx )
{
    NOTIMP;
    return boost::static_pointer_cast<Value>( shared_from_this() );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
