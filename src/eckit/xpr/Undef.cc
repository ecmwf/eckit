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

#include "eckit/xpr/Undef.h"
#include "eckit/xpr/Scope.h"


namespace eckit {
namespace xpr {


Undef::Undef() : Expression()
{
}

Undef::Undef(Stream &s) : Expression(s) {}

Undef::~Undef()
{
}

void Undef::encode(Stream &s) const
{
    Expression::encode(s);
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
    o << "xpr::undef()";
}

ExpPtr Undef::evaluate( Scope &ctx ) const
{
    NOTIMP;
}

ExpPtr Undef::resolve( Scope &ctx ) const
{
   return ctx.nextArg();
}

ExpPtr undef()
{
    return ExpPtr( new Undef() );
}

ExpPtr Undef::cloneWith(args_t& a) const {
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ClassSpec Undef::classSpec_ = {
    &Expression::classSpec(),
    Undef::nodeName().c_str(),
};

Reanimator< Undef > Undef::reanimator_;

} // namespace xpr
} // namespace eckit
