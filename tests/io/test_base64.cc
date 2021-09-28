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

#include "eckit/io/Base64.h"
#include "eckit/io/MemoryHandle.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static std::string test(const char* text, const char* expect) {
    char base64ed[10240] = {};
    char unbase64ed[10240] = {};

    Base64 base64;

    size_t s;
    size_t t;

    std::cout << "Text is [" << text << "]" << std::endl;

    {
        MemoryHandle in(text, strlen(text));
        MemoryHandle out(base64ed, sizeof(base64ed));

        s = base64.encode(in, out);
        std::cout << "Base64 from " << strlen(text) << " to " << s << std::endl;
    }

    std::string v(base64ed, base64ed + s);

    std::cout << "encoded is [" << v << "]" << std::endl;

    EXPECT(v == expect);

    std::cout << "-----------------------" << std::endl;
    {
        MemoryHandle in(base64ed, s);
        MemoryHandle out(unbase64ed, sizeof(unbase64ed));

        t = base64.decode(in, out);
        std::cout << "Unbase64 from " << s << " to " << t << std::endl;
    }

    std::string u(unbase64ed, unbase64ed + t);
    std::cout << "decoded is [" << u << "]" << std::endl;


    std::cout << text << std::endl;
    std::cout << u << std::endl;

    for (size_t i = 0; i < std::min(u.size(), strlen(text)); ++i) {
        std::cout << ((text[i] == u[i]) ? ' ' : '|');
    }
    std::cout << std::endl;

    return u;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_base64_1") {
    {
        const char* pattern = "Man";
        std::string s       = test(pattern, "TWFu");
        EXPECT(s == std::string(pattern));
    }
    {
        const char* pattern = "M";
        std::string s       = test(pattern, "TQ==");
        EXPECT(s == std::string(pattern));
    }
    {
        const char* pattern = "Ma";
        std::string s       = test(pattern, "TWE=");
        EXPECT(s == std::string(pattern));
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
