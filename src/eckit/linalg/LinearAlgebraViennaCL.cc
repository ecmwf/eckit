/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

//-----------------------------------------------------------------------------

#include "eckit/eckit_config.h"

#ifdef ECKIT_HAVE_VIENNACL

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/inner_prod.hpp>
#include <viennacl/linalg/prod.hpp>

#include "eckit/linalg/LinearAlgebraViennaCL.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace linalg {

typedef viennacl::vector<Scalar> vec;
typedef viennacl::matrix<Scalar, viennacl::column_major> mat;
typedef viennacl::compressed_matrix<Scalar> spmat;

//-----------------------------------------------------------------------------

LinearAlgebraViennaCL::LinearAlgebraViennaCL() : LinearAlgebra("viennacl") {}

//-----------------------------------------------------------------------------

void LinearAlgebraViennaCL::print(std::ostream& out) const {
    out << "LinearAlgebraViennaCL[]";
}

//-----------------------------------------------------------------------------

Scalar LinearAlgebraViennaCL::dot(const Vector& x, const Vector& y) const {
    ASSERT( x.size() == y.size() );
    // ViennaCL requires non-const pointers to the data for views
    vec xi(const_cast<Scalar*>(x.data()), viennacl::MAIN_MEMORY, x.size());
    vec yi(const_cast<Scalar*>(y.data()), viennacl::MAIN_MEMORY, y.size());
    return viennacl::linalg::inner_prod(xi, yi);
}

//-----------------------------------------------------------------------------

void LinearAlgebraViennaCL::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    // ViennaCL requires non-const pointers to the data for views
    mat Ai(const_cast<Scalar*>(A.data()), viennacl::MAIN_MEMORY, A.rows(), A.cols());
    vec xi(const_cast<Scalar*>(x.data()), viennacl::MAIN_MEMORY, x.size());
    vec yi(y.data(), viennacl::MAIN_MEMORY, y.size());
    yi = viennacl::linalg::prod(Ai, xi);
}

//-----------------------------------------------------------------------------

void LinearAlgebraViennaCL::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    // ViennaCL requires non-const pointers to the data for views
    mat Ai(const_cast<Scalar*>(A.data()), viennacl::MAIN_MEMORY, A.rows(), A.cols());
    mat Bi(const_cast<Scalar*>(B.data()), viennacl::MAIN_MEMORY, B.rows(), B.cols());
    mat Ci(C.data(), viennacl::MAIN_MEMORY, C.rows(), C.cols());
    Ci = viennacl::linalg::prod(Ai, Bi);
}

//-----------------------------------------------------------------------------

void LinearAlgebraViennaCL::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    spmat Ai;
    // FIXME: this will always copy!
    Ai.set(A.outer(), A.inner(), A.data(), A.rows(), A.cols(), A.nonZeros());
    vec xi(const_cast<Scalar*>(x.data()), viennacl::MAIN_MEMORY, x.size());
    vec yi(y.data(), viennacl::MAIN_MEMORY, y.size());
    yi = viennacl::linalg::prod(Ai, xi);
}

//-----------------------------------------------------------------------------

void LinearAlgebraViennaCL::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    spmat Ai;
    // FIXME: this will always copy!
    Ai.set(A.outer(), A.inner(), A.data(), A.rows(), A.cols(), A.nonZeros());
    // Emulate spmm by looping over columns of B
    for (Size col = 0; col < B.cols(); ++col) {
        vec xi(const_cast<Scalar*>(B.data() + col*B.rows()), viennacl::MAIN_MEMORY, B.rows());
        vec yi(C.data() + col*C.rows(), viennacl::MAIN_MEMORY, C.rows());
        yi = viennacl::linalg::prod(Ai, xi);
    }
}

//-----------------------------------------------------------------------------

void LinearAlgebraViennaCL::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    LinearAlgebra::getBackend("generic").dsptd(x, A, y, B);
}

//-----------------------------------------------------------------------------

static LinearAlgebraViennaCL LinearAlgebraViennaCL;

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit

#endif  // ECKIT_HAVE_VIENNACL
