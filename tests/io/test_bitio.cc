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
#include "eckit/io/BitIO.h"
#include "eckit/io/Buffer.h"

#include "eckit/testing/Setup.h"
#include "eckit/io/MemoryHandle.h"

using namespace std;
using namespace eckit;

using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_bitio )

//----------------------------------------------------------------------------------------------------------------------


BOOST_AUTO_TEST_CASE( test_eckit_bitio_1 ) {

    size_t N = sizeof(size_t) * 8 ;

    Buffer b(17 * 31 * N * sizeof(size_t) * 2);

    {
        MemoryHandle h(b);
        h.openForWrite(0);

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
        h.openForRead();

        BitIO io(h);
        for (size_t k = 0; k < 17; k++) {

            size_t s = 1;
            for (size_t i = 1; i <= N; i++) {
                for (size_t j = 0; j < 31; ++j) {
                    size_t r = io.read(i);
                    if ( r != s) {
                        std::cout << "k=" << k << std::endl;
                        std::cout << "i=" << i << std::endl;
                        std::cout << "j=" << j << std::endl;
                    }
                    BOOST_CHECK_EQUAL(r, s);
                }
                s <<= 1;
            }
        }
    }
}



BOOST_AUTO_TEST_CASE( test_eckit_bitio_2 ) {

    for (size_t nbits = 7; nbits <= 32; ++nbits) {

        // std::cout  << std::dec << "nbits=" << nbits << std::endl;

        const char* pattern = "0123456789";

        Buffer b(1024);

        {
            MemoryHandle h(b);
            h.openForWrite(0);

            BitIO io(h);
            const char*p = pattern;
            while (*p) {
                io.write(*p, nbits);
                p++;
            }
        }

        {
            MemoryHandle h(b);
            h.openForRead();

            BitIO io(h);
            const char*p = pattern;
            while (*p) {
                char c = io.read(nbits);
                BOOST_CHECK_EQUAL(*p, c);
                p++;
            }
        }
    }
}


BOOST_AUTO_TEST_CASE( test_eckit_bitio_3 ) {

    const char* pattern = "TOBEORNOTTOBEORTOBEORNOT#";

    MemoryHandle h(pattern, strlen(pattern));
    h.openForRead();

    BitIO io(h);
    const char*p = pattern;
    while (*p) {
        char c = io.read(8);
        BOOST_CHECK_EQUAL(*p, c);
        p++;
    }

    BOOST_CHECK_EQUAL(io.read(8, 257), 257);

}
//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
