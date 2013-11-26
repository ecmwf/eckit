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

#include "eckit/maths/Expression.h"
#include "eckit/maths/Value.h"
#include "eckit/maths/Scope.h"
#include "eckit/maths/Undef.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/ParamDef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Expression::Expression()
{
}

Expression::Expression(const args_t &args) :
    args_(args)
{
}

Expression::~Expression()
{
}


ExpPtr Expression::eval()
{
    Scope ctx("Expression::eval()");
    return optimise()->evaluate(ctx);
}

ExpPtr Expression::eval( Scope& ctx, bool)
{
    return optimise()->evaluate(ctx);
}


ExpPtr Expression::eval(Scope& scope, const args_t& args)
{
    Scope ctx("Expression::eval(Scope& scope, const args_t& args)", &scope);
    for(args_t::const_iterator j = args.begin(); j != args.end(); ++j) {
        ctx.pushArg(*j);
    }
    return optimise()->evaluate(ctx);
}

ExpPtr Expression::resolve(Scope & ctx)
{
    return self();
}

ExpPtr Expression::eval( ExpPtr e )
{
    Scope ctx("Expression::eval( ExpPtr e )", e);
    ExpPtr res = optimise()->evaluate(ctx);
    return res;
}

ExpPtr Expression::eval(  ExpPtr a, ExpPtr b )
{
    Scope ctx("Expression::eval(  ExpPtr a, ExpPtr b )", a, b);
    ExpPtr res = optimise()->evaluate(ctx);
    return res;
}

ExpPtr Expression::param( size_t i) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );
    return args_[i];
}


ExpPtr Expression::param( size_t i, Scope& ctx ) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );

    //cout << "   PARAM " << i << " -> " << (*args_[i]) << std::endl;
    //cout << "         " << ctx << std::endl;
    ExpPtr r = args_[i]->resolve(ctx);
    //cout << "          " << *r << std::endl;

    return r;
}

void Expression::param(size_t i, ExpPtr p)
{
    ASSERT( i < args_.size() );
    ASSERT( p );
    if( p != args_[i] )
        args_[i] = p;
}

string Expression::str() const
{
    std::ostringstream os;
    print(os);
    return os.str();
}


void Expression::printArgs(std::ostream&out) const
{
    size_t count = arity();
    for(size_t i = 0; i < count; ++i) {
        if(i) out << ", ";
        out << *args_[i];
    }
}

std::ostream& operator<<( std::ostream& os, const Expression& v)
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
