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

#include "eckit/io/BitIO.h"
#include "eckit/io/Buffer.h"

#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_bitio_1") {

    size_t N = sizeof(size_t) * 8;

    Buffer b(17 * 31 * N * sizeof(size_t) * 2);

    {
        MemoryHandle h(b);

        BitIO io(h);
        for (size_t k = 0; k < 17; k++) {
            size_t s = 1;
            for (size_t i = 1; i <= N; i++) {
                for (size_t j = 0; j < 31; ++j) {
                    io.write(s, i);
                }
                s <<= 1;
            }
        }
    }

    {
        MemoryHandle h(b);

        BitIO io(h);
        for (size_t k = 0; k < 17; k++) {

            size_t s = 1;
            for (size_t i = 1; i <= N; i++) {
                for (size_t j = 0; j < 31; ++j) {
                    size_t r = io.read(i);
                    if (r != s) {
                        std::cout << "k=" << k << std::endl;
                        std::cout << "i=" << i << std::endl;
                        std::cout << "j=" << j << std::endl;
                    }
                    EXPECT(r == s);
                }
                s <<= 1;
            }
        }
    }
}


CASE("test_eckit_bitio_2") {

    for (size_t nbits = 7; nbits <= 32; ++nbits) {

        // std::cout  << std::dec << "nbits=" << nbits << std::endl;

        const char* pattern = "0123456789";

        Buffer b(1024);

        {
            MemoryHandle h(b);

            BitIO io(h);
            const char* p = pattern;
            while (*p) {
                io.write(*p, nbits);
                p++;
            }
        }

        {
            MemoryHandle h(b);

            BitIO io(h);
            const char* p = pattern;
            while (*p) {
                char c = io.read(nbits);
                EXPECT(*p == c);
                p++;
            }
        }
    }
}


CASE("test_eckit_bitio_3") {

    const char* pattern = "TOBEORNOTTOBEORTOBEORNOT#";

    MemoryHandle h(pattern, strlen(pattern));

    BitIO io(h);
    const char* p = pattern;
    while (*p) {
        char c = io.read(8);
        EXPECT(*p == c);
        p++;
    }

    EXPECT(io.read(8, 257) == 257);
}

CASE("test_eckit_bitio_4") {

    const char* pattern = "A";

    MemoryHandle h(pattern, strlen(pattern));

    BitIO io(h);

    io.read(6);
    EXPECT_THROWS_AS(io.read(6), std::exception);


    EXPECT(io.read(8, 257) == 257);
}

CASE("test_eckit_bitio_5") {

    const unsigned char pattern[] = {0xff};

    MemoryHandle h(pattern, sizeof(pattern));

    BitIO io(h, true);

    EXPECT(io.read(6, 256) == 0x3f);
    EXPECT(io.read(6, 256) == 0x30);
    EXPECT(io.read(6, 256) == 256);

    EXPECT(io.bitCount() == 8);


    EXPECT(io.read(8, 257) == 257);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
