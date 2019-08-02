/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <iostream>
#include <memory>

#include "eckit/io/Buffer.h"
#include "eckit/io/ResizableBuffer.h"
#include "eckit/utils/Compressor.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

static std::string msg("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK 1234567890");

//----------------------------------------------------------------------------------------------------------------------

std::string tostr(const ResizableBuffer& b, size_t len) {
    return std::string(b, len);
}

size_t compress_uncompress(Compressor& c, const Buffer& in, ResizableBuffer& out) {

    size_t compressedLenght = c.compress(in, out);

    Buffer compressed(out, compressedLenght);

    out.resize(in.size());
    size_t ulen = c.uncompress(compressed, out);

    std::cout << tostr(out, ulen) << std::endl;

    return ulen;
}


CASE("Compression") {

    Buffer in(msg.c_str(), msg.size());
    ResizableBuffer out(msg.size());
    out.zero();

    std::unique_ptr<Compressor> c;

    SECTION("CASE Default Compression") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build()));
        size_t ulen = compress_uncompress(*c, in, out);
        EXPECT(tostr(out, ulen) == msg);
    }

    SECTION("CASE No Compression - case insensitive") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("nOnE")));
    }

    SECTION("Not Existing Compression") { EXPECT_THROWS(c.reset(CompressorFactory::instance().build("dummy name"))); }

    SECTION("CASE No Compression") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("none")));
        size_t ulen = compress_uncompress(*c, in, out);
        EXPECT(tostr(out, ulen) == msg);
    }

    SECTION("CASE Snappy Compression") {

        if (CompressorFactory::instance().has("snappy")) {
            EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("snappy")));
            size_t ulen = compress_uncompress(*c, in, out);
            EXPECT(tostr(out, ulen) == msg);
        }
    }

    SECTION("CASE LZ4 Compression") {
        if (CompressorFactory::instance().has("lz4")) {
            EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("lz4")));
            size_t ulen = compress_uncompress(*c, in, out);
            EXPECT(tostr(out, ulen) == msg);
        }
    }

    SECTION("CASE BZip2 Compression") {
        if (CompressorFactory::instance().has("bzip2")) {
            EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("bzip2")));
            size_t ulen = compress_uncompress(*c, in, out);
            EXPECT(tostr(out, ulen) == msg);
        }
    }

    SECTION("CASE AEC Compression") {
        if (CompressorFactory::instance().has("aec")) {
            EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("aec")));
            size_t ulen = compress_uncompress(*c, in, out);
            EXPECT(tostr(out, ulen) == msg);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
