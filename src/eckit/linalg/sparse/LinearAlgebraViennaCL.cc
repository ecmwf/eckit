/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/sparse/LinearAlgebraViennaCL.h"

#include <ostream>

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/linalg/inner_prod.hpp>
#include <viennacl/linalg/prod.hpp>
#include <viennacl/matrix.hpp>
#include <viennacl/vector.hpp>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {
namespace sparse {


static const LinearAlgebraViennaCL __la("viennacl");


using vec_t = viennacl::vector<Scalar>;
using mat_t = viennacl::matrix<Scalar, viennacl::column_major>;
using spm_t = viennacl::compressed_matrix<Scalar>;


void LinearAlgebraViennaCL::print(std::ostream& out) const {
    out << "LinearAlgebraViennaCL[]";
}


void LinearAlgebraViennaCL::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    // FIXME: this will always copy!
    spm_t Ai;
    Ai.set(A.outer(), A.inner(), A.data(), A.rows(), A.cols(), A.nonZeros());

    vec_t xi(const_cast<Scalar*>(x.data()), viennacl::MAIN_MEMORY, x.size());
    vec_t yi(y.data(), viennacl::MAIN_MEMORY, y.size());
    yi = viennacl::linalg::prod(Ai, xi);
}


void LinearAlgebraViennaCL::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // FIXME: this will always copy!
    spm_t Ai;
    Ai.set(A.outer(), A.inner(), A.data(), A.rows(), A.cols(), A.nonZeros());

    // Emulate spmm by looping over columns of B
    for (Size col = 0; col < B.cols(); ++col) {
        vec_t xi(const_cast<Scalar*>(B.data() + col * B.rows()), viennacl::MAIN_MEMORY, B.rows());
        vec_t yi(C.data() + col * C.rows(), viennacl::MAIN_MEMORY, C.rows());
        yi = viennacl::linalg::prod(Ai, xi);
    }
}


void LinearAlgebraViennaCL::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
