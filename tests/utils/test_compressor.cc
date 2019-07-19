/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <cstring>

#include "eckit/io/Buffer.h"
#include "eckit/utils/Compressor.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

static const char* msg = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK 1234567890";

//----------------------------------------------------------------------------------------------------------------------

CASE("Compression") {

    Buffer in(msg, 1024);
    Buffer out(in.size());
    out.zero();

    std::unique_ptr<Compressor> c;

    SECTION("CASE No Compression") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("None")));

        c->compress(in, out);

        std::cout << (const char*) out.data() << std::endl;

        EXPECT((const char*) out.data() == std::string(msg));
    }

    SECTION("CASE No Compression") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("bzip2")));

//        c->compress(in, out);

//        std::cout << (const char*) out.data() << std::endl;

//        EXPECT((const char*) out.data() == std::string(msg));
    }
}

//----------------------------------------------------------------------------------------------------------------------


}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
