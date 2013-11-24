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

#include "eckit/maths/Param.h"
#include "eckit/maths/Scope.h"

namespace eckit {
namespace maths {


Param::Param(const string& name) : Expr(), name_(name)
{
}

Param::~Param()
{
}

ExpPtr Param::clone()
{
    return parameter(name_);
}

void Param::print(ostream &o) const
{
    o << "_(" << name_ << ")";
}

ExpPtr parameter(const string& name)
{
    return ExpPtr( new Param(name) );
}

ValPtr Param::evaluate( Scope &ctx )
{
    ExpPtr e = ctx.param(name_);
    return e->eval();
}

/*ExpPtr Param::resolve( Scope &ctx )
{
   return ctx.param(name_);
}*/
//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
