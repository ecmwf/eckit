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
#include "eckit/utils/Compressor.h"
#include "eckit/utils/MD5.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

static std::string msg("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK 1234567890");

static std::vector<std::string> compressions{"none", "snappy", "lz4", "bzip2", "aec"};

//----------------------------------------------------------------------------------------------------------------------

void EXPECT_compress_uncompress_1(Compressor& c, const Buffer& in, size_t ulen) {
    // Buffers are not allocated. The compress/uncompress will do so as required
    Buffer compressed;
    Buffer uncompressed;
    size_t clen = c.compress(in, ulen, compressed);
    c.uncompress(compressed, clen, uncompressed, ulen);
    EXPECT(std::memcmp(uncompressed, in, ulen) == 0);
}

void EXPECT_compress_uncompress_2(Compressor& c, const Buffer& in, size_t ulen) {
    // Buffers are pre-allocated. This may allow implementation dependent optimizations
    Buffer compressed(size_t(1.2 * ulen));
    Buffer uncompressed(size_t(1.2 * ulen));
    size_t clen = c.compress(in, ulen, compressed);
    c.uncompress(compressed, clen, uncompressed, ulen);
    EXPECT(std::memcmp(uncompressed, in, ulen) == 0);
}


void EXPECT_reproducible_compression(Compressor& c, size_t times) {
    std::vector<size_t> reproduce_lengths;
    std::vector<std::string> reproduce_checksum;

    for (size_t i = 0; i < times + 1; ++i) {
        Buffer uncompressed(msg.data(), msg.size());
        Buffer compressed;
        size_t compressed_size = c.compress(uncompressed, uncompressed.size(), compressed);
        reproduce_lengths.emplace_back(compressed_size);
        reproduce_checksum.emplace_back(eckit::MD5(compressed, compressed_size));
    }

    const auto& ref_length = reproduce_lengths.front();
    bool reproducible_length{true};
    for (auto& length : reproduce_lengths) {
        if (length != ref_length) {
            reproducible_length = false;
        }
    }
    EXPECT(reproducible_length);

    const auto& ref_checksum = reproduce_checksum.front();
    bool reproducible_checksum{true};
    for (auto& checksum : reproduce_checksum) {
        if (checksum != ref_checksum) {
            reproducible_checksum = false;
        }
    }
    EXPECT(reproducible_checksum);
}

CASE("Builders") {
    std::unique_ptr<Compressor> c;

    SECTION("CASE No Compression - case insensitive") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build("nOnE")));
    }

    SECTION("Not Existing Compression") {
        EXPECT_THROWS(c.reset(CompressorFactory::instance().build("dummy name")));
    }
}

CASE("Compression") {

    Buffer in(2 * msg.size());  // oversized on purpose
    in.copy(msg.c_str(), msg.size());
    const size_t len = msg.size();  // valid size

    std::unique_ptr<Compressor> c;

    SECTION("CASE Default Compression") {
        EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build()));
        EXPECT_compress_uncompress_1(*c, in, len);
        EXPECT_compress_uncompress_2(*c, in, len);
    }

    for (const auto& compression : compressions) {
        SECTION("CASE " + compression) {
            if (CompressorFactory::instance().has(compression)) {
                EXPECT_NO_THROW(c.reset(CompressorFactory::instance().build(compression)));
                EXPECT_compress_uncompress_1(*c, in, len);
                EXPECT_compress_uncompress_2(*c, in, len);
                EXPECT_reproducible_compression(*c, 10);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
