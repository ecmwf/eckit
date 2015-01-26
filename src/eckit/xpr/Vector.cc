/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Vector::Vector( const size_t& s, const scalar_t& v )
    : v_(s,v)
{
}

Vector::Vector( const value_t& v )
    : v_(v)
{
}

Vector::Vector(Vector::value_t &v, Swap ignored )
{
    std::swap(v_,v);
}

Vector::Vector(Stream &s) : Value(s), v_()
{
    size_t l;
    s >> l;
    v_.resize(l);
    for( size_t i = 0; i < l; ++i ) {
        s >> v_[i];
    }
}

void Vector::encode(Stream &s) const
{
    Value::encode(s);
    s << v_.size();
    for( size_t i = 0; i < v_.size(); ++i ) {
        s << v_[i];
    }
}

bool Vector::is(const ExpPtr &e)
{
    return dynamic_cast<Vector*>(e.get()) != 0;
}

ExpPtr Vector::cloneWith(args_t& a) const
{
    NOTIMP; // Should not be called
}

void Vector::print(std::ostream&o) const
{
    o << nodeName() << "(";
    for( size_t i = 0; i < v_.size(); ++i )
    {
        if(i) o << ", ";
        o << v_[i];
    }
    o << ")";
}

void Vector::asCode(std::ostream&o) const
{
    o << "xpr::vector(";
    for( size_t i = 0; i < v_.size(); ++i )
    {
        if(i) o << ", ";
        o << v_[i];
    }
    o << ")";
}

//--------------------------------------------------------------------------------------------

ClassSpec Vector::classSpec_ = {
    &Value::classSpec(),
    Vector::nodeName().c_str(),
};

Reanimator< Vector > Vector::reanimator_;

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

} // namespace xpr
} // namespace eckit
