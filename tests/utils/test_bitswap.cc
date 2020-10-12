/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <vector>

#include "eckit/utils/BitSwap.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

template< typename T >
std::vector<T> get_ref() {
    std::vector<T> ref(10);
    for( size_t i=0; i<ref.size(); ++i ) {
        ref[i] = T(i);
    }
    return ref;
}

template< typename T> 
void test_roundtrip() {
    const auto ref = get_ref<T>();
    auto v = ref;

    // First roundtrip with array API
    eckit::bitswap(v);
    EXPECT( v != ref );
    eckit::bitswap(v.data(),v.size());
    EXPECT( v == ref );
    
    // Second roundtrip with scalar API
    for( auto& x : v ) {
      x = eckit::bitswap(x);
    }
    EXPECT( v != ref );
    eckit::bitswap(v);
    EXPECT( v == ref );
}

CASE( "test_roundtrip" ) {
    SECTION("int") {
        test_roundtrip<int>();
    }
    SECTION("long") {
        test_roundtrip<long>();
    }
    SECTION("float") {
        test_roundtrip<float>();
    }
    SECTION("double") {
        test_roundtrip<double>();
    }
}


}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
