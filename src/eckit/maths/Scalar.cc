/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Scalar::Scalar( const scalar_t& v ) : v_(v)
{
}

void Scalar::print(ostream &o) const
{
    o << class_name() << "(" << v_ << ")";
}

Scalar::Scalar(const ExpPtr& e) : v_(0)
{
   ASSERT( e->ret_signature() == Scalar::sig() );
   v_ = Scalar::extract( e->evaluate() );
}

//--------------------------------------------------------------------------------------------

ExpPtr scalar(const scalar_t &s)
{
    return ExpPtr( new Scalar(s) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
