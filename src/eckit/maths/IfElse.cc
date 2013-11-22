/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/IfElse.h"
#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

//static Func::RegisterFactory< IfElse > fmap_register;

IfElse::IfElse(const args_t& args) : Func(args)
{
}

IfElse::IfElse( ExpPtr f,  ExpPtr i, ExpPtr e ) : Func()
{
    ASSERT( Scalar::is( f ) );

    args_.push_back(f);
    args_.push_back(i);
    args_.push_back(e);
}

string IfElse::ret_signature() const
{
    string i = param(1)->ret_signature();
    string e = param(2)->ret_signature();

    if(i == e) {
        return i;
    }
    else {
        return "?";
    }
}

ValPtr IfElse::evaluate( context_t& ctx )
{
    ExpPtr f = param(0, &ctx);
    ExpPtr v = f->eval();

    ExpPtr i = param(1, &ctx);
    ExpPtr e = param(2, &ctx);

    scalar_t b = Scalar::extract( v );

    if(b) {
        return i->eval();
    }
    else {
        return e->eval();
    }


}

//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f,  ExpPtr i, ExpPtr e )
{
    return ExpPtr( new IfElse(f, i, e) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
