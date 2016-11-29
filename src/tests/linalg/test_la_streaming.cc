/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_la_streaming

#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/FileStream.h"

#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "util.h"

using namespace eckit::linalg;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
void test(const T& v, const T& r) {
    BOOST_CHECK_EQUAL(v.rows(), r.rows());
    BOOST_CHECK_EQUAL(v.cols(), r.cols());
    BOOST_CHECK_EQUAL(v.size(), r.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), r.begin(), r.end());
}

void test(const SparseMatrix& v, const SparseMatrix& r) {
    BOOST_CHECK_EQUAL(v.rows(), r.rows());
    BOOST_CHECK_EQUAL(v.cols(), r.cols());
    BOOST_CHECK_EQUAL(v.nonZeros(), r.nonZeros());
    const Size nnz = v.nonZeros();
    BOOST_CHECK_EQUAL_COLLECTIONS(v.outer(), v.outer()+v.rows()+1, r.outer(), r.outer()+r.rows()+1);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.inner(), v.inner()+nnz, r.inner(), r.inner()+nnz);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.data(), v.data()+nnz, r.data(), r.data()+nnz);
}

template<typename T>
void stream_test(const T& t) {
    PathName filename = PathName::unique( "data" );
    {
        FileStream sout( filename, "w" );
        sout << t;
    }
    {
        FileStream sin( filename, "r" );
        T out(sin);
        test(t, out);
    }
    if (filename.exists()) filename.unlink();
}

//----------------------------------------------------------------------------------------------------------------------

/// Test linear algebra interface

BOOST_AUTO_TEST_SUITE(test_eckit_la_streaming)

BOOST_AUTO_TEST_CASE(test_stream_vector) {
    stream_test(V(5, 1., 2., 3., 4., 5.));
}

BOOST_AUTO_TEST_CASE(test_stream_matrix) {
    stream_test(M(3, 3, 1., 2., 3., 4., 5., 6., 7., 8., 9.));
}

BOOST_AUTO_TEST_CASE(test_stream_sparsematrix) {

    std::vector<Triplet> triplets;

    triplets.push_back(Triplet(0, 0, 2.));
    triplets.push_back(Triplet(0, 2, -3.));
    triplets.push_back(Triplet(1, 1, 2.));
    triplets.push_back(Triplet(2, 2, 2.));

    SparseMatrix smat(3, 3, triplets);

    stream_test(smat);
}

BOOST_AUTO_TEST_SUITE_END()

//----------------------------------------------------------------------------------------------------------------------

} // namespace test
} // namespace eckit
