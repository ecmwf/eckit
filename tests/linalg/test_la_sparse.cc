/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"

#include "./util.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::linalg;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

SparseMatrix S(Size rows, Size cols, Size nnz, ...) {
    va_list args;
    va_start(args, nnz);
    std::vector<Triplet> triplets;
    for (Size n = 0; n < nnz; ++n) {
        Size row = Size( va_arg(args, int) );
        Size col = Size( va_arg(args, int) );
        Scalar v = va_arg(args, Scalar);
        triplets.push_back(Triplet(row, col, v));
    }
    va_end(args);

    SparseMatrix mat(rows, cols, triplets);

//    ECKIT_DEBUG_VAR(mat.nonZeros());
//    ECKIT_DEBUG_VAR(mat.rows());
//    ECKIT_DEBUG_VAR(mat.cols());

    return mat;
}

//----------------------------------------------------------------------------------------------------------------------

struct Fixture {

    Fixture() : A(S(3, 3, 4,
                    0, 0, 2.,
                    0, 2, -3.,
                    1, 1, 2.,
                    2, 2, 2.)),

                A2(S(2, 3, 4,
                     0, 0, 1.,
                     0, 2, 2.,
                     1, 0, 3.,
                     1, 1, 4.)),

                x(V(3, 1., 2., 3.)),

                linalg(LinearAlgebra::backend()) {}

    // A = 2 0 -3
    //     0 2  0
    //     0 0  2
    SparseMatrix A;
    // A2 = 1. 0. 2.
    //      3. 4. 0.
    SparseMatrix A2;
    Vector x;
    const LinearAlgebra& linalg;
};

template <class T>
void test(const T& v, const T& r) {
    const size_t s = std::min(v.size(), r.size());
    EXPECT( is_approximately_equal( make_view( v.data(), s), make_view( r.data(), s ), 0.1 ) );
}

template <typename T>
void test(T* v, T* r, size_t s) {
    EXPECT ( make_view( v, s ) == make_view( r, s ) );
}

void test(const SparseMatrix& A, const Index* outer, const Index* inner, const Scalar* data) {
    test(A.outer(), outer, A.rows()+1);
    test(A.inner(), inner, A.nonZeros());
    test(A.data(), data, A.nonZeros());
}


/// Test linear algebra interface

CASE ( "test_eckit_la_sparse" ) {

    Fixture F;

    SECTION ( "test_set_from_triplets" ) {
    {

        // A = 2 0 -3
        //     0 2  0
        //     0 0  2

        EXPECT( F.A.nonZeros() == 4 );

        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 2, 1, 2};
        Scalar data[4] = {2., -3., 2., 2.};
        test(F.A, outer, inner, data);
    }

    // Pathological case with empty rows
    {
        Index outer[7] = {0, 0, 1, 1, 2, 2, 2};
        Index inner[2] = {0, 3};
        Scalar data[2] = {1., 2.};
        test(S(6, 6, 2, 1, 0, 1., 3, 3, 2.), outer, inner, data);
    }}
    // Rows in wrong order (not triggering right now since triplets are sorted)
    // EXPECT_THROWS_AS( S(2, 2, 2, 1, 1, 1., 0, 0, 1.), AssertionFailed );

    SECTION ( "test_set_copy_constructor" ) {
    {
        SparseMatrix B(F.A);

        EXPECT( B.nonZeros() == 4 );

        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 2, 1, 2};
        Scalar data[4] = {2., -3., 2., 2.};
        test(B, outer, inner, data);
    }}

    SECTION ( "test_identity" ) {
    {
        Vector y1(3);

        SparseMatrix B;
        B.setIdentity(3, 3);

        F.linalg.spmv(B, F.x, y1);
        test(y1, F.x);
    }

    {
        SparseMatrix C;
        C.setIdentity(6, 3);

        Vector y2(6);
        F.linalg.spmv(C, F.x, y2);
        test(y2, F.x);
        test(y2.data()+3, V(3, 0., 0., 0.).data(), 3);
    }

    {
        SparseMatrix D;
        D.setIdentity(2, 3);

        Vector y3(2);

        F.linalg.spmv(D, F.x, y3);
        test(y3, F.x);
    }}

    SECTION ( "test_prune" ) {

        SparseMatrix A(S(3, 3, 5,
                        0, 0, 0.,
                        0, 2, 1.,
                        1, 0, 0.,
                        1, 1, 2.,
                        2, 2, 0.));

        A.prune();
        EXPECT ( A.nonZeros() == 2 );
        Index outer[4] = {0, 1, 2, 2};
        Index inner[2] = {2, 1};
        Scalar data[2] = {1., 2.};
        test(A, outer, inner, data);
    }

    SECTION ( "rowReduction" ) {

        SparseMatrix A(S(4, 3, 6,
                        0, 0, 2.,
                        0, 2, 1.,
                        1, 0, 7.,
                        1, 1, 2.,
                        2, 2, 1.,
                        3, 1, 3.));

        // A
        // 2 . 1
        // 7 2 .
        // . . 1
        // . 3 .

        vector<size_t> p;
        p.push_back(1);
        p.push_back(0);

        SparseMatrix B = A.rowReduction(p);

        // B
        // 7 2 .
        // 2 . 1

        EXPECT ( B.rows() == p.size() );
        EXPECT ( B.nonZeros() == 4 );

         B.dump(Log::info());

        Index outer[3] = {0, 2, 4};
        Index inner[4] = {0, 1, 0, 2};
        Scalar data[4] = {7., 2., 2., 1.};
        test(B, outer, inner, data);
    }

    SECTION ( "test_iterator" ) {

        SparseMatrix A(S(3, 3, 5,
                        0, 0, 0.,
                        1, 0, 0.,
                        1, 1, 0.,
                        1, 2, 1.,
                        2, 2, 2.));

        A.prune();
        EXPECT( A.nonZeros() == 2 );

        //  data    [ 1 2 ]
        //  outer   [ 0 0 1 2 ]
        //  inner   [ 2 2 ]

        Scalar data[2] = {1., 2.};
        Index outer[4] = {0, 0, 1, 2};
        Index inner[2] = {2, 2};
        test(A, outer, inner, data);

        SparseMatrix::const_iterator it = A.begin();

        // check entry #1
        EXPECT( it.row() == 1 );
        EXPECT( it.col() == 2 );
        EXPECT( *it == 1. );

        // check entry #2
        ++it;

        EXPECT( it.row() == 2 );
        EXPECT( it.col() == 2 );
        EXPECT( *it == 2. );

        // go past the end
        EXPECT( it != A.end() );

        ++it;

        EXPECT( it == A.end() );
        EXPECT( !it );

        // go back and re-check entry #1
        // (row 0 is empty, should relocate to row 1)
        it = A.begin();
        EXPECT( it );

        EXPECT( it.row() == 1 );
        EXPECT( it.col() == 2 );
        EXPECT( *it == 1. );

        // go way past the end
        it = A.begin(42);
        EXPECT( !it );

    }

    SECTION ( "test_transpose_square" ) {
        Index outer[4] = {0, 1, 2, 4};
        Index inner[4] = {0, 1, 0, 2};
        Scalar data[4] = {2., 2., -3., 2.};
        test( F.A.transpose(), outer, inner, data );
    }

    SECTION ( "test_transpose_nonsquare" ) {
        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 1, 1, 0};
        Scalar data[4] = {1., 3., 4., 2.};
        test( F.A2.transpose(), outer, inner, data );
    }

    SECTION ( "test_spmv" ) {
        Vector y(3);
        F.linalg.spmv( F.A, F.x, y);
        test(y, V(3, -7., 4., 6.));
        Log::info() << "spmv of sparse matrix and vector of nonmatching sizes should fail" << std::endl;
        EXPECT_THROWS_AS( F.linalg.spmv(F.A, Vector(2), y), AssertionFailed );
    }

    SECTION ( "test_spmm" ) {
        Matrix C(3, 2);
        F.linalg.spmm( F.A, M(3, 2, 1., 2., 3., 4., 5., 6.), C);
        test(C, M(3, 2, -13., -14., 6., 8., 10., 12.));
        Log::info() << "spmm of sparse matrix and matrix of nonmatching sizes should fail" << std::endl;
        EXPECT_THROWS_AS( F.linalg.spmm(F.A, Matrix(2, 2), C), AssertionFailed );
    }

    SECTION ( "test_dsptd_square" ) {
        SparseMatrix B;
        F.linalg.dsptd(F.x, F.A, F.x, B);
        Index outer[4] = {0, 1, 2, 4};
        Index inner[4] = {0, 1, 0, 2};
        Scalar data[4] = {2., 8., -9., 18.};
        test(B, outer, inner, data);
        Log::info() << "dsptd with vectors of nonmatching sizes should fail" << std::endl;
        EXPECT_THROWS_AS( F.linalg.dsptd(F.x, F.A, Vector(2), B), AssertionFailed );
    }

    SECTION ( "test_dsptd_nonsquare" ) {
        SparseMatrix B;
        F.linalg.dsptd(F.x, F.A2, V(2, 1., 2.), B);
        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 1, 1, 0};
        Scalar data[4] = {1., 6., 16., 6.};
        test(B, outer, inner, data);
        Log::info() << "dsptd with vectors of nonmatching sizes should fail" << std::endl;
        EXPECT_THROWS_AS( F.linalg.dsptd(F.x, F.A2, F.x, B), AssertionFailed );
    }
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    eckit::Main::initialise( argc, argv );
    // Set linear algebra backend
    LinearAlgebra::backend(Resource<std::string>("-linearAlgebraBackend", "generic"));

    return run_tests ( argc, argv, false );
}
