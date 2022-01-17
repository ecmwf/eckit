/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/serialisation/FileStream.h"
#include "util.h"


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
void test(const T& v, const T& r) {
    EXPECT(v.rows() == r.rows());
    EXPECT(v.cols() == r.cols());
    EXPECT(v.size() == r.size());
    EXPECT(testing::make_view(v.begin(), v.end()) == testing::make_view(r.begin(), r.end()));
}

void test(const linalg::SparseMatrix& v, const linalg::SparseMatrix& r) {
    EXPECT(v.rows() == r.rows());
    EXPECT(v.cols() == r.cols());
    EXPECT(v.nonZeros() == r.nonZeros());
    const auto nnz = v.nonZeros();
    EXPECT(testing::make_view(v.outer(), v.outer() + v.rows() + 1) == testing::make_view(r.outer(), r.outer() + r.rows() + 1));
    EXPECT(testing::make_view(v.inner(), v.inner() + nnz) == testing::make_view(r.inner(), r.inner() + nnz));
    EXPECT(testing::make_view(v.data(), v.data() + nnz) == testing::make_view(r.data(), r.data() + nnz));
}

template <typename T>
void stream_test(const T& t) {
    PathName filename = PathName::unique("data");
    {
        FileStream sout(filename, "w");
        auto c = closer(sout);
        sout << t;
    }
    {
        FileStream sin(filename, "r");
        auto c = closer(sin);
        T out(sin);
        test(t, out);
    }
    if (filename.exists()) {
        filename.unlink();
    }
}

//----------------------------------------------------------------------------------------------------------------------

/// Test linear algebra interface

CASE("test_stream_vector") {
    stream_test(V(5, 1., 2., 3., 4., 5.));
}

CASE("test_stream_matrix") {
    stream_test(M(3, 3, 1., 2., 3., 4., 5., 6., 7., 8., 9.));
}

CASE("test_stream_sparsematrix") {
    std::vector<eckit::linalg::Triplet> triplets;

    triplets.emplace_back(0, 0, 2.);
    triplets.emplace_back(0, 2, -3.);
    triplets.emplace_back(1, 1, 2.);
    triplets.emplace_back(2, 2, 2.);

    eckit::linalg::SparseMatrix smat(3, 3, triplets);

    stream_test(smat);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
