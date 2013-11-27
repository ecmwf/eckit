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

bool Scalar::is(const ExpPtr &e) {
    return dynamic_cast<Scalar*>(e.get()) != 0;
}

ExpPtr Scalar::clone() const
{
    return maths::scalar( value() );
}

void Scalar::print(ostream &o) const
{
    o << className() << "(" << v_ << ")";
}

Scalar::Scalar(const ExpPtr& e) : v_(0)
{
   //ASSERT( e->returnSignature() == Scalar::sig() );
   v_ = Scalar::extract( e->eval() );
}

//--------------------------------------------------------------------------------------------

ExpPtr scalar(const scalar_t &s)
{
    return ExpPtr( new Scalar(s) );
}

void Scalar::asCode(ostream &o) const
{
    o << "Math(" << v_ << ")";
}


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
