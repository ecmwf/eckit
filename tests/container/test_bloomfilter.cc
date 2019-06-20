/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/container/BloomFilter.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_container_bloomfilter_insert") {

    BloomFilter<std::string> f(1024);

    EXPECT(f.empty());
    EXPECT(!f.contains("hello there"));
    EXPECT(!f.contains("hello there again"));

    f.insert("hello there");

    EXPECT(f.contains("hello there"));
    EXPECT(!f.contains("hello there again"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
