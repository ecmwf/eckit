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

#include "eckit/io/Compress.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static std::string test(const char* text) {
    char compressed[10240] = {};
    char uncompressed[10240] = {};

    Compress compress(16);

    size_t s;
    size_t t;

    std::cout << "Text is [" << text << "]" << std::endl;

    {
        MemoryHandle in(text, strlen(text));
        MemoryHandle out(compressed, sizeof(compressed));

        s = compress.encode(in, out);
        std::cout << "Compress from " << strlen(text) << " to " << s << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
    {
        MemoryHandle in(compressed, s);
        MemoryHandle out(uncompressed, sizeof(uncompressed));

        t = compress.decode(in, out);
        std::cout << "Uncompress from " << s << " to " << t << std::endl;
    }

    std::string u(uncompressed, uncompressed + t);

    std::cout << text << std::endl;
    std::cout << u << std::endl;

    for (size_t i = 0; i < std::min(u.size(), strlen(text)); ++i) {
        std::cout << ((text[i] == u[i]) ? ' ' : '|');
    }
    std::cout << std::endl;


    return u;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_compress_1") {
    const char* pattern = "TOBEORNOTTOBEORTOBEORNOT#";

    std::string s = test(pattern);
    EXPECT(s == std::string(pattern));
}


CASE("test_eckit_compress_2") {
    const char* pattern = "ABCABCABCABCABCABC";

    std::string s = test(pattern);
    EXPECT(s == std::string(pattern));
}

CASE("test_eckit_compress_3") {
    const char* pattern = "";

    std::string s = test(pattern);
    EXPECT(s == std::string(pattern));
}


CASE("test_eckit_compress_4") {

    char original[102400];
    char compressed[102400 * 2];
    char uncompressed[102400];

    size_t s;
    size_t t;

    for (size_t i = 0; i < sizeof(original); ++i) {
        original[i] = i;
    }

    for (size_t bits = 16; bits >= 9; bits--) {
        std::cout << "Bits " << bits << std::endl;
        Compress compress(bits);

        {
            MemoryHandle in(original, sizeof(original));
            MemoryHandle out(compressed, sizeof(compressed));

            s = compress.encode(in, out);

            std::cout << "Compress from " << sizeof(original) << " to " << s << " "
                      << int(double(s) / double(sizeof(original)) * 100.0 + 0.5) << "%" << std::endl;
        }

        std::cout << "-----------------------" << std::endl;
        {
            MemoryHandle in(compressed, s);
            MemoryHandle out(uncompressed, sizeof(uncompressed));

            t = compress.decode(in, out);
            std::cout << "Uncompress from " << s << " to " << t << std::endl;
        }

        EXPECT(sizeof(original) == t);

        for (size_t i = 0; i < sizeof(original); ++i) {
            EXPECT(original[i] == uncompressed[i]);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
