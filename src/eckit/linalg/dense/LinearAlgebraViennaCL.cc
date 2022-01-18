/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/dense/LinearAlgebraViennaCL.h"

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/linalg/inner_prod.hpp>
#include <viennacl/linalg/prod.hpp>
#include <viennacl/matrix.hpp>
#include <viennacl/vector.hpp>

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {
namespace dense {


static const LinearAlgebraViennaCL __la("viennacl");


using vec_t = viennacl::vector<Scalar>;
using mat_t = viennacl::matrix<Scalar, viennacl::column_major>;


void LinearAlgebraViennaCL::print(std::ostream& out) const {
    out << "LinearAlgebraViennaCL[]";
}


Scalar LinearAlgebraViennaCL::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    // ViennaCL requires non-const pointers to the data for views
    vec_t xi(const_cast<Scalar*>(x.data()), viennacl::MAIN_MEMORY, x.size());
    vec_t yi(const_cast<Scalar*>(y.data()), viennacl::MAIN_MEMORY, y.size());

    return viennacl::linalg::inner_prod(xi, yi);
}


void LinearAlgebraViennaCL::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    // ViennaCL requires non-const pointers to the data for views
    mat_t Ai(const_cast<Scalar*>(A.data()), viennacl::MAIN_MEMORY, A.rows(), A.cols());
    vec_t xi(const_cast<Scalar*>(x.data()), viennacl::MAIN_MEMORY, x.size());
    vec_t yi(y.data(), viennacl::MAIN_MEMORY, y.size());

    yi = viennacl::linalg::prod(Ai, xi);
}


void LinearAlgebraViennaCL::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // ViennaCL requires non-const pointers to the data for views
    mat_t Ai(const_cast<Scalar*>(A.data()), viennacl::MAIN_MEMORY, A.rows(), A.cols());
    mat_t Bi(const_cast<Scalar*>(B.data()), viennacl::MAIN_MEMORY, B.rows(), B.cols());
    mat_t Ci(C.data(), viennacl::MAIN_MEMORY, C.rows(), C.cols());

    Ci = viennacl::linalg::prod(Ai, Bi);
}


}  // namespace dense
}  // namespace linalg
}  // namespace eckit
