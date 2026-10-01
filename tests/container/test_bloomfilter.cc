// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/container/BloomFilter.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

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

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
