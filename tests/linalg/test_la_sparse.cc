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
#include "util.h"

using namespace eckit::linalg;

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

struct Fixture {
    const Vector x;
    const SparseMatrix A;
    const Vector y;
};

/// Test sparse matrix interface

CASE("move constructor") {
    SparseMatrix a{S(3, 3, 4, 0, 0, 2., 0, 2, -3., 1, 1, 2., 2, 2, 2.)};
    EXPECT(!a.empty());

    SparseMatrix b{std::move(a)};
    EXPECT(!b.empty());
}

CASE("eckit la sparse") {
    // "square" fixture
    // A =  2  . -3
    //      .  2  .
    //      .  .  2
    // x = 1 2 3
    // y = 1 2 3
    Fixture F{V(3, 1., 2., 3.), S(3, 3, 4, 0, 0, 2., 0, 2, -3., 1, 1, 2., 2, 2, 2.), V(3, 1., 2., 3.)};

    // "non-square" fixture
    // A = 1  .  2
    //     3  4  .
    // x = 1 2
    // y = 1 2 3
    Fixture G{V(2, 1., 2.), S(2, 3, 4, 0, 0, 1., 0, 2, 2., 1, 0, 3., 1, 1, 4.), V(3, 1., 2., 3.)};

    SECTION("set from triplets") {
        // A = 2 0 -3
        //     0 2  0
        //     0 0  2

        EXPECT(F.A.nonZeros() == 4);

        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 2, 1, 2};
        Scalar data[4] = {2., -3., 2., 2.};
        EXPECT(equal_sparse_matrix(F.A, outer, inner, data));
    }

    SECTION("set from triplets with empty rows") {
        Index outer[7] = {0, 0, 1, 1, 2, 2, 2};
        Index inner[2] = {0, 3};
        Scalar data[2] = {1., 2.};
        EXPECT(equal_sparse_matrix(S(6, 6, 2, 1, 0, 1., 3, 3, 2.), outer, inner, data));
    }

    SECTION("set from triplets with rows in wrong order") {
        //  (not triggering right now since triplets are sorted)
        // EXPECT_THROWS_AS( S(2, 2, 2, 1, 1, 1., 0, 0, 1.), AssertionFailed );
    }

    SECTION("copy constructor") {
        SparseMatrix B(F.A);

        EXPECT(B.nonZeros() == 4);

        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 2, 1, 2};
        Scalar data[4] = {2., -3., 2., 2.};
        EXPECT(equal_sparse_matrix(B, outer, inner, data));
    }

    SECTION("prune") {
        SparseMatrix A(S(3, 3, 5, 0, 0, 0., 0, 2, 1., 1, 0, 0., 1, 1, 2., 2, 2, 0.));
        A.prune();

        EXPECT(A.nonZeros() == 2);

        Index outer[4] = {0, 1, 2, 2};
        Index inner[2] = {2, 1};
        Scalar data[2] = {1., 2.};
        EXPECT(equal_sparse_matrix(A, outer, inner, data));
    }

    SECTION("row reduction") {
        SparseMatrix A(S(4, 3, 6, 0, 0, 2., 0, 2, 1., 1, 0, 7., 1, 1, 2., 2, 2, 1., 3, 1, 3.));

        // A
        // 2 . 1
        // 7 2 .
        // . . 1
        // . 3 .

        std::vector<size_t> p{1, 0};
        SparseMatrix B = A.rowReduction(p);

        // B
        // 7 2 .
        // 2 . 1

        EXPECT(B.rows() == p.size());
        EXPECT(B.nonZeros() == 4);

        B.dump(Log::info());

        Index outer[3] = {0, 2, 4};
        Index inner[4] = {0, 1, 0, 2};
        Scalar data[4] = {7., 2., 2., 1.};
        EXPECT(equal_sparse_matrix(B, outer, inner, data));
    }

    SECTION("iterator") {
        SparseMatrix A(S(3, 3, 5, 0, 0, 0., 1, 0, 0., 1, 1, 0., 1, 2, 1., 2, 2, 2.));

        A.prune();
        EXPECT(A.nonZeros() == 2);

        //  data    [ 1 2 ]
        //  outer   [ 0 0 1 2 ]
        //  inner   [ 2 2 ]

        Scalar data[2] = {1., 2.};
        Index outer[4] = {0, 0, 1, 2};
        Index inner[2] = {2, 2};
        EXPECT(equal_sparse_matrix(A, outer, inner, data));

        SparseMatrix::const_iterator it = A.begin();

        // check entry #1
        EXPECT(it.row() == 1);
        EXPECT(it.col() == 2);
        EXPECT(*it == 1.);

        // check entry #2
        ++it;

        EXPECT(it.row() == 2);
        EXPECT(it.col() == 2);
        EXPECT(*it == 2.);

        // go past the end
        EXPECT(it != A.end());

        ++it;

        EXPECT(it == A.end());
        EXPECT(!it);

        // go back and re-check entry #1
        // (row 0 is empty, should relocate to row 1)
        it = A.begin();
        EXPECT(it);

        EXPECT(it.row() == 1);
        EXPECT(it.col() == 2);
        EXPECT(*it == 1.);

        // go way past the end
        it = A.begin(42);
        EXPECT(!it);
    }

    SECTION("transpose square") {
        Index outer[4] = {0, 1, 2, 4};
        Index inner[4] = {0, 1, 0, 2};
        Scalar data[4] = {2., 2., -3., 2.};
        SparseMatrix B(F.A);
        EXPECT(equal_sparse_matrix(B.transpose(), outer, inner, data));
    }

    SECTION("transpose non-square") {
        Index outer[4] = {0, 2, 3, 4};
        Index inner[4] = {0, 1, 1, 0};
        Scalar data[4] = {1., 3., 4., 2.};
        SparseMatrix B(G.A);
        EXPECT(equal_sparse_matrix(B.transpose(), outer, inner, data));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("creation with unassigned triplets ( ECKIT-361 )") {
    Size N{10};
    Size M{8};
    Size max_stencil_size = 4;

    SECTION("only zero triplets, expects throw") {
        std::vector<Triplet> triplets(N * max_stencil_size);
        EXPECT_THROWS(SparseMatrix matrix(N, M, triplets));
    }

    SECTION("mixed zero / non-zero triplets") {
        auto compute_row_triplets = [&](Size row) {
            std::vector<Triplet> row_triplets(3);
            for (Size i = 0; i < 3; ++i) {
                row_triplets[i] = Triplet(row, i, 1. / 3.);
            }
            return row_triplets;
        };
        auto skip_point = [](Size row) {
            if (row == 5)
                return true;
            return false;
        };

        std::vector<Triplet> triplets(N * max_stencil_size);

        Size nonzeros{0};
        for (Size i = 0; i < N; ++i) {
            if (!skip_point(i)) {
                auto row = compute_row_triplets(i);
                for (Size j = 0; j < row.size(); ++j) {
                    triplets[i * max_stencil_size + j] = row[j];
                    ++nonzeros;
                }
            }
        }
        SparseMatrix matrix(N, M, triplets);
        EXPECT(matrix.rows() == N);
        EXPECT(matrix.cols() == M);
        EXPECT(matrix.nonZeros() == nonzeros);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);
    return eckit::testing::run_tests(argc, argv, false);
}
