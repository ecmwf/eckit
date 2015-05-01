/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Dot.h"
#include "eckit/xpr/Matrix.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

Dot::Dot( ExpPtr l,  ExpPtr r ) : Function()
{
    push_back(l);
    push_back(r);
}

Dot::Dot( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

Dot::Dot(Stream &s) : Function(s) {}

ExpPtr Dot::cloneWith(args_t& a) const
{
    return ExpPtr( new Dot(a) );
}

//-----------------------------------------------------------------------------

ClassSpec Dot::classSpec_ = {
    &Function::classSpec(),
    Dot::nodeName(),
};

Reanimator< Dot > Dot::reanimator_;

//-----------------------------------------------------------------------------

ExpPtr dot( ExpPtr l, ExpPtr r )
{
    return ExpPtr( new Dot(l,r) );
}

//-----------------------------------------------------------------------------

ExpPtr compute_vec_vec(Scope&, const args_t &p)
{
    Vector::value_t v1 = Vector::extract(p[0]);
    Vector::value_t v2 = Vector::extract(p[1]);
    ASSERT( v1.size() == v2.size() );
    Vector::elemt_t r;
    for( size_t i = 0; i < v1.size(); ++i )
        r += v1[i] * v2[i];
    return real(r);
}

ExpPtr compute_mat_vec(Scope&, const args_t &p)
{
    Matrix::value_t M = Matrix::extract(p[0]);
    Vector::value_t v = Vector::extract(p[1]);
    size_t rows = Matrix::rows(p[0]);
    size_t cols = Matrix::cols(p[0]);
    ASSERT( v.size() == cols );
    Vector::value_t rv(rows, 0.);
    for( size_t r = 0; r < rows; ++r )
        for (size_t c = 0; c < cols; ++c )
            rv[r] += M[r*cols+c] * v[c];
    return ExpPtr( new Vector(rv, Expression::Swap()) );
}

ExpPtr compute_mat_mat(Scope&, const args_t &p)
{
    Matrix::value_t A = Matrix::extract(p[0]);
    Matrix::value_t B = Matrix::extract(p[1]);
    size_t rowsA = Matrix::rows(p[0]);
    size_t colsA = Matrix::cols(p[0]);
    size_t rowsB = Matrix::rows(p[1]);
    size_t colsB = Matrix::cols(p[1]);
    ASSERT( colsA == rowsB );
    Matrix::value_t C(rowsA*colsB, 0.);
    for( size_t r = 0; r < rowsA; ++r )
        for( size_t c = 0; c < colsB; ++c )
            for( size_t k = 0; k < colsA; ++k )
                C[r*rowsA+c] += A[r*rowsA+k] * B[k*rowsB+c];
    return ExpPtr( new Matrix(rowsA, colsB, C, Expression::Swap()) );
}

struct DotRegister
{
    DotRegister() {
        Function::dispatcher()["xpr::Dot(v,v)"] = &compute_vec_vec;
        Function::dispatcher()["xpr::Dot(m,v)"] = &compute_mat_vec;
        Function::dispatcher()["xpr::Dot(m,m)"] = &compute_mat_mat;
    }
};

DotRegister dotRegister;

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
