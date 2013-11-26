/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Boolean.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Boolean::Boolean( const Boolean::value_t& v ) : v_(v)
{
}

ExpPtr Boolean::clone()
{
    return maths::boolean( value() );
}

bool Boolean::is(const ExpPtr &e) {
    return dynamic_cast<Boolean*>(e.get()) != 0;
}

void Boolean::print(ostream &o) const
{
    o << className() << "(" << v_ << ")";
}

void Boolean::asCode(ostream &o) const
{
    o << "Math(" << (v_? "true" : "false") << ")";
}

Boolean::Boolean(const ExpPtr& e) : v_(0)
{
   //ASSERT( e->returnSignature() == Boolean::sig() );
   v_ = Boolean::extract( e->eval() );
}

//--------------------------------------------------------------------------------------------

ExpPtr boolean(const Boolean::value_t &s)
{
    return ExpPtr( new Boolean(s) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
