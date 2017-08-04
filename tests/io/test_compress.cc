/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_parser

#include "ecbuild/boost_test_framework.h"

//#include "eckit/log/Log.h"
#include "eckit/io/Compress.h"

#include "eckit/testing/Setup.h"
#include "eckit/io/MemoryHandle.h"

using namespace std;
using namespace eckit;

using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_compress )

//----------------------------------------------------------------------------------------------------------------------

static std::string test(const char* text) {
    char compressed[10240] = {0,};
    char uncompressed[10240] = {0,};

    size_t s;
    size_t t;

    std::cout << "Text is [" << text << "]" << std::endl;

    {
        MemoryHandle in(text, strlen(text));
        MemoryHandle out(compressed, sizeof(compressed));

        in.openForRead();
        out.openForWrite(0);

        s = Compress::encode(in, out);
        std::cout << "Compress from " << strlen(text) << " to " << s << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
    {
        MemoryHandle in(compressed, s);
        MemoryHandle out(uncompressed, sizeof(uncompressed));

        in.openForRead();
        out.openForWrite(0);

        t = Compress::decode(in, out);
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


BOOST_AUTO_TEST_CASE( test_eckit_compress_1 ) {
    const char* pattern = "TOBEORNOTTOBEORTOBEORNOT#";
    // const char* pattern = "ABCABCABCABCABCABC";

    std::string s = test(pattern);
    BOOST_CHECK_EQUAL(s, std::string(pattern));
}


//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
