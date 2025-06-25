/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/Literals.h"

#include "eckit/testing/Test.h"

using namespace eckit::literals;
using namespace eckit::testing;

namespace eckit::test {

CASE("size literals") {
    EXPECT_EQUAL(1_KiB, 1ULL << 10);
    EXPECT_EQUAL(1_MiB, 1ULL << 20);
    EXPECT_EQUAL(1_GiB, 1ULL << 30);
    EXPECT_EQUAL(1_TiB, 1ULL << 40);
    EXPECT_EQUAL(1_PiB, 1ULL << 50);
    EXPECT_EQUAL(1_EiB, 1ULL << 60);
}
}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
