/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <array>

#include "eckit/maths/Matrix.h"
#include "eckit/types/FloatCompare.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::types;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_determinant") {
    typedef eckit::maths::Matrix<double> Matrix;

    Matrix m(5, 5);
    m(0, 0) = 9.;
    m(0, 1) = 6.;
    m(0, 2) = 2.;
    m(0, 3) = 0.;
    m(0, 4) = 3.;
    m(1, 0) = 3.;
    m(1, 1) = 6.;
    m(1, 2) = 8.;
    m(1, 3) = 10.;
    m(1, 4) = 12.;
    m(2, 0) = 4.;
    m(2, 1) = 8.;
    m(2, 2) = 2.;
    m(2, 3) = 6.;
    m(2, 4) = 9.;
    m(3, 0) = 1.;
    m(3, 1) = 5.;
    m(3, 2) = 5.;
    m(3, 3) = 3.;
    m(3, 4) = 2.;
    m(4, 0) = 1.;
    m(4, 1) = 3.;
    m(4, 2) = 6.;
    m(4, 3) = 8.;
    m(4, 4) = 10.;

    EXPECT(is_approximately_equal(m.determinant(), 1124., 1.e-10));
}

//----------------------------------------------------------------------------------------------------------------------
CASE("test_inverse") {
    typedef eckit::maths::Matrix<double> Matrix;

    Matrix m(5, 5);
    m(0, 0) = 9.;
    m(0, 1) = 6.;
    m(0, 2) = 2.;
    m(0, 3) = 0.;
    m(0, 4) = 3.;
    m(1, 0) = 3.;
    m(1, 1) = 6.;
    m(1, 2) = 8.;
    m(1, 3) = 10.;
    m(1, 4) = 12.;
    m(2, 0) = 4.;
    m(2, 1) = 8.;
    m(2, 2) = 2.;
    m(2, 3) = 6.;
    m(2, 4) = 9.;
    m(3, 0) = 1.;
    m(3, 1) = 5.;
    m(3, 2) = 5.;
    m(3, 3) = 3.;
    m(3, 4) = 2.;
    m(4, 0) = 1.;
    m(4, 1) = 3.;
    m(4, 2) = 6.;
    m(4, 3) = 8.;
    m(4, 4) = 10.;


    // Expected inverse
    Matrix minv(5, 5);
    minv(0, 0) = 0.03024911;
    minv(0, 1) = 0.78113879;
    minv(0, 2) = -0.1405694;
    minv(0, 3) = -0.29181495;
    minv(0, 4) = -0.76156584;
    minv(1, 0) = -0.01067616;
    minv(1, 1) = -0.33451957;
    minv(1, 2) = 0.16725979;
    minv(1, 3) = 0.22064057;
    minv(1, 4) = 0.20996441;
    minv(2, 0) = 0.12366548;
    minv(2, 1) = -0.54181495;
    minv(2, 2) = -0.10409253;
    minv(2, 3) = 0.27758007;
    minv(2, 4) = 0.65124555;
    minv(3, 0) = -0.31939502;
    minv(3, 1) = 2.07562278;
    minv(3, 2) = -0.16281139;
    minv(3, 3) = -0.5658363;
    minv(3, 4) = -2.13523132;
    minv(4, 0) = 0.18149466;
    minv(4, 1) = -1.31316726;
    minv(4, 2) = 0.15658363;
    minv(4, 3) = 0.24911032;
    minv(4, 4) = 1.43060498;

    // Compute inverse
    Matrix mi(5, 5);
    mi.noalias() = m.inverse();

    // Compute identity matrix
    Matrix I(5, 5);
    I = m * mi;

    // Verify results
    for (int i = 0; i < m.rows(); ++i) {
        for (int j = 0; j < m.cols(); ++j) {
            EXPECT(is_approximately_equal(mi(i, j), minv(i, j), 1.e-4));
            if (i == j)
                EXPECT(is_approximately_equal(I(i, j), 1., 1.e-8));
            else
                EXPECT(is_approximately_equal(I(i, j), 0., 1.e-10));
        }
    }
}

CASE("test matrix operations") {
    typedef eckit::maths::Matrix<double> Matrix;

    SECTION("bounds") {

        const auto A = Matrix{
            {1., 2.},
            {3., 4.},
            {5., 6.}};

        EXPECT_EQUAL(A.rows(), 3);
        EXPECT_EQUAL(A.cols(), 2);
        EXPECT_EQUAL(A.size(), 6);

        EXPECT_EQUAL(A(0, 0), 1.);
        EXPECT_EQUAL(A(0, 1), 2.);
        EXPECT_EQUAL(A(1, 0), 3.);
        EXPECT_EQUAL(A(1, 1), 4.);
        EXPECT_EQUAL(A(2, 0), 5.);
        EXPECT_EQUAL(A(2, 1), 6.);
    }

    SECTION("addition") {

        const auto A = Matrix{
            {1., 2.},
            {3., 4.}};

        const auto B = Matrix{
            {5., 6.},
            {7., 8.}};

        const auto APlusB = Matrix{
            {6., 8.},
            {10., 12.}};

        EXPECT(A + B == APlusB);
        EXPECT(APlusB - B == A);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "B = " << std::endl
                    << B << std::endl;
        Log::info() << "A + B = " << std::endl
                    << A + B << std::endl
                    << std::endl;
    }

    SECTION("multiplication") {

        const auto A = Matrix{
            {1., 0., 1.},
            {2., 1., 1.},
            {0., 1., 1.},
            {1., 1., 2.}};

        const auto B = Matrix{
            {1., 2., 1.},
            {2., 3., 1.},
            {4., 2., 2.}};

        const auto AB = Matrix{
            {5., 4., 3.},
            {8., 9., 5.},
            {6., 5., 3.},
            {11., 9., 6.}};

        EXPECT(A * B == AB);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "B = " << std::endl
                    << B << std::endl;
        Log::info() << "A x B = " << std::endl
                    << A * B << std::endl
                    << std::endl;
    }


    SECTION("inverse") {

        const auto A = Matrix{
            {-1., 1.5},
            {1., -1.}};

        const auto invA = Matrix{
            {2., 3.},
            {2., 2.}};

        EXPECT(A.inverse() == invA);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "A^-1 = " << std::endl
                    << A.inverse() << std::endl
                    << std::endl;
    }

    SECTION("transpose") {

        const auto A = Matrix{
            {1., 2.},
            {3., 4.}};


        const auto At = Matrix{
            {1., 3.},
            {2., 4.}};

        EXPECT(A.transpose() == At);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "A^T = " << std::endl
                    << A.transpose() << std::endl
                    << std::endl;
    }

    SECTION("determinant") {

        const auto A = Matrix{
            {3., 7.},
            {1., -4.}};

        EXPECT(is_approximately_equal(A.determinant(), -19.));

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "|A| = " << std::endl
                    << A.determinant() << std::endl
                    << std::endl;
    }

    SECTION("row/col slice") {

        const auto A = Matrix{
            {1., 2., 1.},
            {2., 3., 1.},
            {4., 2., 2.}};

        const auto ARow0 = Matrix{
            {1., 2., 1.}};

        const auto ACol0 = Matrix{
            {1.},
            {2.},
            {4.}};

        EXPECT(A.row(0) == ARow0);
        EXPECT(A.col(0) == ACol0);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "A(0, j) = " << std::endl
                    << ARow0 << std::endl;
        Log::info() << "A(i, 0) = " << std::endl
                    << ACol0 << std::endl
                    << std::endl;
    }

    SECTION("matrix times scalar") {

        const auto A = Matrix{
            {1., 2.},
            {3., 4.}};


        const auto ATimes2 = Matrix{
            {2., 4.},
            {6., 8.}};

        EXPECT(A * 2 == ATimes2);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "2A = " << std::endl
                    << ATimes2 << std::endl
                    << std::endl;
    }

    SECTION("coefficient wise product") {

        const auto A = Matrix{
            {1., 2.},
            {3., 4.}};

        const auto B = Matrix{
            {5., 6.},
            {7., 8.}};

        const auto ATimesB = Matrix{
            {5., 12.},
            {21., 32.}};

        EXPECT(A.cwiseProduct(B) == ATimesB);
        EXPECT(B.cwiseProduct(A) == ATimesB);

        Log::info() << "A = " << std::endl
                    << A << std::endl;
        Log::info() << "B = " << std::endl
                    << B << std::endl;
        Log::info() << "cwiseProd(A, B) = " << std::endl
                    << A.cwiseProduct(B) << std::endl
                    << std::endl;
    }
}

CASE("Mappings to existing data") {
    using Matrix    = maths::Matrix<double>;
    using ColVector = maths::ColVector<double>;
    using RowVector = maths::RowVector<double>;
    std::array<double, 6> matrix{1, 3, 5, 2, 4, 6};  // 3x2 column-major layout

    auto A = Matrix::ConstMapType(matrix.data(), 3, 2);

    EXPECT_EQUAL(A(0, 0), 1.);
    EXPECT_EQUAL(A(0, 1), 2.);
    EXPECT_EQUAL(A(1, 0), 3.);
    EXPECT_EQUAL(A(1, 1), 4.);
    EXPECT_EQUAL(A(2, 0), 5.);
    EXPECT_EQUAL(A(2, 1), 6.);

    // y = A * x
    {
        std::array<double, 2> vector_x{1, 2};
        std::array<double, 3> vector_y;

        auto x = ColVector::ConstMapType(vector_x.data(), 2);
        EXPECT_EQUAL(x(0), 1.);
        EXPECT_EQUAL(x(1), 2.);

        auto y = ColVector::MapType(vector_y.data(), 3);
        y      = A * x;
        EXPECT_EQUAL(vector_y[0], 5.);
        EXPECT_EQUAL(vector_y[1], 11.);
        EXPECT_EQUAL(vector_y[2], 17.);
    }

    // y = x * A
    {
        std::array<double, 3> vector_x{1, 2, 3};
        std::array<double, 2> vector_y;

        auto x = RowVector::ConstMapType(vector_x.data(), 3);
        EXPECT_EQUAL(x(0), 1.);
        EXPECT_EQUAL(x(1), 2.);
        EXPECT_EQUAL(x(2), 3.);

        auto y = RowVector::MapType(vector_y.data(), 2);
        y      = x * A;
        EXPECT_EQUAL(vector_y[0], 22.);
        EXPECT_EQUAL(vector_y[1], 28.);
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
