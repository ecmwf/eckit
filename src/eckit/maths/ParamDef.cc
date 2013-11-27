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

#include "eckit/maths/ParamDef.h"
#include "eckit/maths/Scope.h"

namespace eckit {
namespace maths {


ParamDef::ParamDef(const string& name) : Expression(), name_(name)
{
}

ParamDef::~ParamDef()
{
}

ExpPtr ParamDef::resolve(Scope & ctx) const
{
    return ctx.nextArg();
}


void ParamDef::print(std::ostream&o) const
{
    o << "=(" << name_ << ")";
}

ExpPtr paramdef(const string& name)
{
    return ExpPtr( new ParamDef(name) );
}

ExpPtr ParamDef::evaluate( Scope &ctx ) const
{
    NOTIMP;
}

void ParamDef::asCode(std::ostream&o) const
{
    o << '"' << name_ << '"';
}


bool ParamDef::is(const ExpPtr &e) {
    return dynamic_cast<ParamDef*>(e.get()) != 0;
}



//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
