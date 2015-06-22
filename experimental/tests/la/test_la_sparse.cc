/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_la_linalg

#include "ecbuild/boost_test_framework.h"

#include "eckit/runtime/Context.h"
#include "eckit/config/Resource.h"

#include "eckit/la/LinearAlgebraFactory.h"
#include "eckit/la/SparseMatrix.h"
#include "eckit/la/Vector.h"
#include "util.h"

//-----------------------------------------------------------------------------

using namespace eckit::la;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

// Required for Resources to be initialised
struct Setup {
    Setup() {
        Context::instance().setup(boost::unit_test::framework::master_test_suite().argc,
                                  boost::unit_test::framework::master_test_suite().argv);
    }
};

BOOST_GLOBAL_FIXTURE(Setup);

//-----------------------------------------------------------------------------

struct Fixture {

    Fixture() : S(3, 3),
                x(V(3, 1., 2., 3.)),
                linalg(LinearAlgebraFactory::get(Resource<std::string>("--backend", "generic"))) {
        std::vector<Triplet> triplets;
        triplets.push_back(Triplet(0, 0, 2.));
        triplets.push_back(Triplet(0, 2, -3.));
        triplets.push_back(Triplet(1, 1, 2.));
        triplets.push_back(Triplet(2, 2, 2.));
        S.setFromTriplets(triplets);
    }

    SparseMatrix S;
    Vector x;
    LinearAlgebraBase* linalg;
};

//-----------------------------------------------------------------------------

template <class T>
void test(const T& v, const T& r) {
    const size_t s = std::min(v.size(), r.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(v.data(), v.data() + s, r.data(), r.data() + s);
}

template <typename T>
void test(T* v, T* r, size_t s) {
    BOOST_CHECK_EQUAL_COLLECTIONS(v, v + s, r, r + s);
}

//-----------------------------------------------------------------------------

/// Test linear algebra interface

BOOST_FIXTURE_TEST_SUITE(test_eckit_la_sparse, Fixture)

BOOST_AUTO_TEST_CASE(test_create_sparse_from_triplets) {
    BOOST_CHECK_EQUAL(S.nonZeros(), 4);
    Index outer[4] = {0, 2, 3, 4};
    Index inner[4] = {0, 2, 1, 2};
    Scalar data[4] = {2., -3., 2., 2.};
    test(S.outer(), outer, 4);
    test(S.inner(), inner, 4);
    test(S.data(), data, 4);
}

BOOST_AUTO_TEST_CASE(test_identity) {
    Vector y1(3);
    SparseMatrix A(3, 3);
    A.setIdentity();
    linalg->spmv(A, x, y1);
    test(y1, x);
    SparseMatrix B(6, 3);
    B.setIdentity();
    Vector y2(6);
    linalg->spmv(B, x, y2);
    test(y2, x);
    test(y2.data()+3, V(3, 0., 0., 0.).data(), 3);
    SparseMatrix C(2, 3);
    C.setIdentity();
    Vector y3(2);
    linalg->spmv(C, x, y3);
    test(y3, x);
}

BOOST_AUTO_TEST_CASE(test_prune) {
    S.data()[1] = 0.;
    S.prune();
    BOOST_CHECK_EQUAL(S.nonZeros(), 3);
    Index outer[4] = {0, 1, 2, 3};
    Index inner[3] = {0, 1, 2};
    Scalar data[3] = {2., 2., 2.};
    test(S.outer(), outer, 4);
    test(S.inner(), inner, 3);
    test(S.data(), data, 3);
}

BOOST_AUTO_TEST_CASE(test_spmv) {
    Vector y(3);
    linalg->spmv(S, x, y);
    test(y, V(3, -7., 4., 6.));
    BOOST_TEST_MESSAGE("spmv of sparse matrix and vector of nonmatching sizes should fail");
    BOOST_CHECK_THROW(linalg->spmv(S, Vector(2), y), AssertionFailed);
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckittest
