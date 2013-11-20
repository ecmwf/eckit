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

void Boolean::print(ostream &o) const
{
    o << class_name() << "(" << v_ << ")";
}

Boolean::Boolean(const ExpPtr& e) : v_(0)
{
   ASSERT( e->ret_signature() == Boolean::sig() );
   context_t ctx;
   v_ = Boolean::extract( e->evaluate(ctx) );
}

//--------------------------------------------------------------------------------------------

ExpPtr boolean(const Boolean::value_t &s)
{
    return ExpPtr( new Boolean(s) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
