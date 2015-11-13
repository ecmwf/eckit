/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cstdio>

#define BOOST_TEST_MODULE TestStreamable
#include "ecbuild/boost_test_framework.h"

#include "eckit/serialisation/FileStream.h"

using namespace std;
using namespace eckit;

namespace eckit {
namespace test {

namespace {

const char               i_char('a');
const unsigned char      i_uchar('b');
const bool               i_bool(true);
const int                i_int(-20000000);
const unsigned int       i_uint(30000000);
const short              i_short(-4000);
const unsigned short     i_ushort(5000);
const long               i_long(-60000000);
const unsigned long      i_ulong(70000000);
const long long          i_longlong(-80000000);
const unsigned long long i_ulonglong(90000000);
//const float              i_float(300000);
const double             i_double(100000000);
const string             i_string("abcdefghijklmnopqrstuvwx");
const char*              i_charp("cdefghijklmnopqrstuvwxyz");

char               v_char;
unsigned char      v_uchar;
bool               v_bool;
int                v_int;
unsigned int       v_uint;
short              v_short;
unsigned short     v_ushort;
long               v_long;
unsigned long      v_ulong;
long long          v_longlong;
unsigned long long v_ulonglong;
//float              v_float;
double             v_double;
string             v_string;
string             v_charp;

}  // anonymous namespace

struct F {
    ~F()
    {
        if (filename.exists()) filename.unlink();
    }

    static PathName filename;
};

PathName F::filename = PathName::unique( "data" );


BOOST_GLOBAL_FIXTURE( F );

BOOST_AUTO_TEST_SUITE( TestFileStream )

BOOST_AUTO_TEST_CASE( write_data )
{
    BOOST_TEST_MESSAGE("Write to FileStream");
    FileStream sout( F::filename.asString().c_str(), "w" );

    sout << i_char
         << i_uchar
         << i_bool
         << i_int
         << i_uint
         << i_short
         << i_ushort
         << i_long
         << i_ulong
         << i_longlong
         << i_ulonglong
//         << i_float
         << i_double
         << i_string
         << i_charp
    ;
}

BOOST_AUTO_TEST_CASE( read_data )
{
    BOOST_TEST_MESSAGE("Read from FileStream");
    FileStream sin( F::filename.asString().c_str(), "r" );

    sin
        >> v_char
        >> v_uchar
        >> v_bool
        >> v_int
        >> v_uint
        >> v_short
        >> v_ushort
        >> v_long
        >> v_ulong
        >> v_longlong
        >> v_ulonglong
    //  >> v_float
        >> v_double
        >> v_string
        >> v_charp
    ;
}

BOOST_AUTO_TEST_CASE( check_data )
{
    BOOST_CHECK_EQUAL( v_char,      i_char );
    BOOST_CHECK_EQUAL( v_uchar,     i_uchar );
    BOOST_CHECK_EQUAL( v_bool,      i_bool );
    BOOST_CHECK_EQUAL( v_int,       i_int );
    BOOST_CHECK_EQUAL( v_uint,      i_uint );
    BOOST_CHECK_EQUAL( v_short,     i_short );
    BOOST_CHECK_EQUAL( v_ushort,    i_ushort );
    BOOST_CHECK_EQUAL( v_long,      i_long );
    BOOST_CHECK_EQUAL( v_ulong,     i_ulong );
    BOOST_CHECK_EQUAL( v_longlong,  i_longlong );
    BOOST_CHECK_EQUAL( v_ulonglong, i_ulonglong );
//  BOOST_CHECK_EQUAL( v_float,     i_float );
    BOOST_CHECK_EQUAL( v_double,    i_double );
    BOOST_CHECK_EQUAL( v_string,    i_string );
    BOOST_CHECK_EQUAL( v_charp,     i_charp );
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace eckit
