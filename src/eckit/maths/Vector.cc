/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Vector.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Vector::Vector( const size_t& s, const scalar_t& v )
    : v_(s,v)
{
}

Vector::Vector( const value_t& v )
    : v_(v)
{
}

void Vector::print(ostream &o) const
{
    o << class_name() << "(";
    for( size_t i = 0; i < v_.size(); ++i )
    {
        if(i) o << ", ";
        o << v_[i];
    }
    o << ")";
}

//--------------------------------------------------------------------------------------------

ExpPtr vector( const size_t& sz, const scalar_t& v )
{
    return ExpPtr( new Vector(sz,v) );
}

ExpPtr vector( const Vector::value_t& v  )
{
    return ExpPtr( new Vector(v) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
