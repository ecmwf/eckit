/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/JSON.h"
#include "eckit/types/Types.h"

#include "eckit/xpr/Matrix.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Matrix::Matrix( const size_t& rows, const size_t& cols, const real_t& v )
    : v_(rows*cols,v), rows_(rows), cols_(cols)
{
}

Matrix::Matrix( const size_t& rows, const size_t& cols, const value_t& v )
    : v_(v), rows_(rows), cols_(cols)
{
    ASSERT( v.size() == rows*cols );
}

Matrix::Matrix(const size_t & rows, const size_t & cols, Matrix::value_t &v, Swap ignored )
    : rows_(rows), cols_(cols)
{
    ASSERT( v.size() == rows*cols );
    std::swap(v_,v);
}

Matrix::Matrix( const size_t& rows, const size_t& cols,
                const std::function< real_t(size_t, size_t) >& init )
    : v_(rows*cols), rows_(rows), cols_(cols)
{
    for (size_t c = 0; c < rows; ++c)
        for (size_t r = 0; r < cols; ++r)
            v_[r+rows*c] = init(r, c);
}

Matrix::Matrix(Stream &s) : Value(s)
{
    s >> rows_;
    s >> cols_;
    s >> v_;
}

void Matrix::encode(Stream &s) const
{
    Value::encode(s);
    s << rows_;
    s << cols_;
    s << v_;
}

bool Matrix::is(const ExpPtr &e)
{
    return dynamic_cast<Matrix*>(e.get()) != 0;
}

ExpPtr Matrix::cloneWith(args_t& a) const
{
    NOTIMP; // Should not be called
}

void Matrix::print(std::ostream&o) const
{
    o << nodeName() << "(" << rows_ << ", " << cols_ << ")";
}

void Matrix::asCode(std::ostream&o) const
{
    o << factoryName() << "(" << rows_ << ", " << cols_ << ", {";
    for( size_t i = 0; i < v_.size(); ++i )
    {
        if(i) o << ", ";
        o << v_[i];
    }
    o << "})";
}

void Matrix::asJSON(JSON& s) const
{
    s.startObject();
    s << factoryName() << rows_ << cols_ << v_;
    s.endObject();
}

//--------------------------------------------------------------------------------------------

ClassSpec Matrix::classSpec_ = {
    &Value::classSpec(),
    Matrix::nodeName(),
};

Reanimator< Matrix > Matrix::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr matrix( const size_t& rows, const size_t& cols, const real_t& v )
{
    return ExpPtr( new Matrix(rows, cols, v) );
}

ExpPtr matrix( const size_t& rows, const size_t& cols, const Matrix::value_t& v  )
{
    return ExpPtr( new Matrix(rows, cols, v) );
}

ExpPtr matrix( const size_t& rows, const size_t& cols, const std::initializer_list<real_t> v )
{
    return ExpPtr( new Matrix(rows, cols, Matrix::value_t(v)) );
}

ExpPtr matrix( const size_t& rows, const size_t& cols,
               const std::function< real_t(size_t, size_t) >& initialiser )
{
    return ExpPtr( new Matrix(rows, cols, initialiser ) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
