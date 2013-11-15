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

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Expression::Expression()
{}

Expression::Expression(const args_t &args) : args_(args)
{}

Expression::~Expression()
{}

ValPtr Expression::eval()
{
    context_t ctx;
    return reduce()->evaluate(ctx);
}
ValPtr Expression::eval( ExpPtr e )
{
    context_t ctx;
    ctx.push_back(e);
    ExpPtr res = reduce()->evaluate(ctx);
    ASSERT( ctx.empty() );
    return res;
}

ValPtr Expression::eval(  ExpPtr a, ExpPtr b )
{
    context_t ctx;
    ctx.push_back(a);
    ctx.push_back(b);
    ExpPtr res = reduce()->evaluate(ctx);
    ASSERT( ctx.empty() );
    return res;
}

ExpPtr Expression::param(const size_t &i, context_t& ctx) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );

    if( args_[i] == ExpPtr::undef )
    {
        ASSERT( !ctx.empty() );
        ExpPtr e = ctx.front();
        ctx.pop_front();
        return e;
    }

    return args_[i];
}

string Expression::str() const
{
    std::ostringstream os;
    print(os);
    return os.str();
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
