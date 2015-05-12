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
#include "eckit/types/Types.h"

#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Vector::Vector( const size_t& s, const real_t& v )
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

Vector::Vector( const size_t& s, const std::function< real_t(size_t) >& init )
    : v_(s)
{
    for (size_t i = 0; i < s; ++i) v_[i] = init(i);
}

Vector::Vector(Stream &s) : Value(s), v_()
{
    s >> v_;
}

void Vector::encode(Stream &s) const
{
    Value::encode(s);
    s << v_;
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
    if ( v_.size() < 10 ) {
        for( size_t i = 0; i < v_.size(); ++i ) {
            if(i) o << ", ";
            o << v_[i];
        }
    } else {
        o << v_[0] << ", " << v_[1] << ", ..., " << v_[v_.size()-1];
    }
    o << ")";
}

void Vector::asCode(std::ostream&o) const
{
    o << factoryName() << "({";
    for( size_t i = 0; i < v_.size(); ++i )
    {
        if(i) o << ", ";
        o << v_[i];
    }
    o << "})";
}

void Vector::asJSON(JSON& s) const
{
    s << v_;
}

//--------------------------------------------------------------------------------------------

ClassSpec Vector::classSpec_ = {
    &Value::classSpec(),
    Vector::nodeName(),
};

Reanimator< Vector > Vector::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr vector( const size_t& sz, const real_t& v )
{
    return ExpPtr( new Vector(sz,v) );
}

ExpPtr vector( const Vector::value_t& v  )
{
    return ExpPtr( new Vector(v) );
}

ExpPtr vector( const std::initializer_list<real_t> v )
{
    return ExpPtr( new Vector(Vector::value_t(v)) );
}

ExpPtr vector( const size_t& sz, const std::function< real_t(size_t) >& init )
{
    return ExpPtr( new Vector(sz, init) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
