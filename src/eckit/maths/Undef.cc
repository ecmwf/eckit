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

#include "eckit/maths/Undef.h"
#include "eckit/maths/Scope.h"


namespace eckit {
namespace maths {



Undef::Undef() : Expression()
{
}

Undef::~Undef()
{
}

bool Undef::is(const ExpPtr &e) {
    return dynamic_cast<Undef*>(e.get()) != 0;
}

void Undef::print(std::ostream&o) const
{
    o << "?";
}

void Undef::asCode(std::ostream&o) const
{
    o << "maths::undef()";
}

ExpPtr Undef::evaluate( Scope &ctx ) const
{
    NOTIMP;
}

ExpPtr Undef::resolve( Scope &ctx ) const
{
   return ctx.nextArg();
}

ExpPtr Undef::clone() const
{
    return undef();
}

ExpPtr undef()
{
    return ExpPtr( new Undef() );
}


} // namespace maths
} // namespace eckit
