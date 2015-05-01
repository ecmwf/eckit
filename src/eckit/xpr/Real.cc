/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/JSON.h"

#include "eckit/xpr/Real.h"
#include "eckit/xpr/Scope.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Real::Real( const real_t& v ) : v_(v)
{
}

Real::Real(Stream &s) : Value(s), v_(0)
{
    s >> v_;
}

void Real::encode(Stream &s) const
{
    Value::encode(s);
    s << v_;
}

bool Real::is(const ExpPtr &e)
{
    return dynamic_cast<Real*>(e.get()) != 0;
}

void Real::print(std::ostream&o) const
{
    o << nodeName() << "(" << v_ << ")";
}

Real::Real(ExpPtr e) : v_(0)
{
   v_ = Real::extract( e->eval(false) );
}

ExpPtr Real::cloneWith(args_t& a) const {
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ClassSpec Real::classSpec_ = {
    &Value::classSpec(),
    Real::nodeName(),
};

Reanimator< Real > Real::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr real(const real_t &s)
{
    return ExpPtr( new Real(s) );
}

void Real::asCode(std::ostream&o) const
{
    o << "xpr::real(" << v_ << ")";
}

void Real::asJSON(JSON& s) const
{
    s << v_;
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
