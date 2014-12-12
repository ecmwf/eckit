/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/String.h"
#include "eckit/xpr/Scope.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

String::String( const std::string& v ) : v_(v)
{
}

bool String::is(const ExpPtr &e)
{
    return dynamic_cast<String*>(e.get()) != 0;
}

void String::print(std::ostream&o) const
{
    o << className() << "(" << v_ << ")";
}

String::String(ExpPtr e) : v_(0)
{
   Scope dummy("String::String");
   v_ = String::extract(dummy, e->eval(dummy) );
}

ExpPtr String::cloneWith(args_t& a) const {
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ExpPtr string(const std::string &s)
{
    return ExpPtr( new String(s) );
}

void String::asCode(std::ostream&o) const
{
    o << "xpr::string(" << v_ << ")";
}


//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
