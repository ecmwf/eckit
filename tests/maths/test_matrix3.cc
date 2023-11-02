/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <algorithm>

#include "eckit/maths/Matrix3.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::test {


using Matrix = maths::Matrix3<double>;

constexpr double tolerance = 1.e-8;


//----------------------------------------------------------------------------------------------------------------------


CASE("test determinant") {
    Matrix M{1,
             2,
             3,  //
             4,
             5,
             6,  //
             7,
             8,
             1};

    EXPECT(types::is_approximately_equal(M.determinant(), 24., tolerance));
}


CASE("test inverse") {
    auto is_approximately_equal = [](const Matrix& A, const Matrix& B) {
        ASSERT(A.size() == B.size());
        return std::equal(A.begin(), A.end(), B.begin(), [](double a, double b) {
            return types::is_approximately_equal(a, b, tolerance);
        });
    };

    Matrix M{1,
             2,
             3,  //
             4,
             5,
             6,  //
             7,
             8,
             1};

    // Calculate inverse
    auto W = M.inverse();
    EXPECT(is_approximately_equal(W,
                                  {-43. / 24.,
                                   22. / 24.,
                                   -3. / 24.,  //
                                   38. / 24.,
                                   -20. / 24.,
                                   6. / 24.,  //
                                   -3. / 24.,
                                   6. / 24.,
                                   -3. / 24.}));

    // Calculate identity
    auto I = Matrix::identity();
    EXPECT(is_approximately_equal(I,
                                  {1,
                                   0,
                                   0,  //
                                   0,
                                   1,
                                   0,  //
                                   0,
                                   0,
                                   1}));

    EXPECT(is_approximately_equal(I, W * M));
    EXPECT(is_approximately_equal(I, M * W));
}


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
