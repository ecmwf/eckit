/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/types.h>
#include <cstdio>
#include <cstdlib>

#include "eckit/io/AutoCloser.h"
#include "eckit/serialisation/FileStream.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

const char i_char('a');
const unsigned char i_uchar('b');
const bool i_bool(true);
const int i_int(-20000000);
const unsigned int i_uint(30000000);
const short i_short(-4000);
const unsigned short i_ushort(5000);
const long i_long(-60000000);
const unsigned long i_ulong(70000000);
const long long i_longlong(-80000000);
const unsigned long long i_ulonglong(90000000);
// const float              i_float(300000);
const double i_double(100000000);
const string i_string("abcdefghijklmnopqrstuvwx");
const char* i_charp("cdefghijklmnopqrstuvwxyz");

char v_char;
unsigned char v_uchar;
bool v_bool;
int v_int;
unsigned int v_uint;
short v_short;
unsigned short v_ushort;
long v_long;
unsigned long v_ulong;
long long v_longlong;
unsigned long long v_ulonglong;
// float              v_float;
double v_double;
string v_string;
string v_charp;

}  // anonymous namespace

struct F {
    ~F() {
        if (filename.exists())
            filename.unlink();
    }

    static PathName filename;
};

PathName F::filename = PathName::unique("data");

//----------------------------------------------------------------------------------------------------------------------

CASE("write_data") {
    FileStream sout(F::filename, "w");
    auto c = closer(sout);
    sout << i_char << i_uchar << i_bool << i_int << i_uint << i_short << i_ushort << i_long << i_ulong << i_longlong
         << i_ulonglong
         //         << i_float
         << i_double << i_string << i_charp;
}

CASE("read_data") {
    FileStream sin(F::filename, "r");
    auto c = closer(sin);
    sin >> v_char >> v_uchar >> v_bool >> v_int >> v_uint >> v_short >> v_ushort >> v_long >> v_ulong >> v_longlong >> v_ulonglong
        //  >> v_float
        >> v_double >> v_string >> v_charp;
}

CASE("check_data") {
    EXPECT(v_char == i_char);
    EXPECT(v_uchar == i_uchar);
    EXPECT(v_bool == i_bool);
    EXPECT(v_int == i_int);
    EXPECT(v_uint == i_uint);
    EXPECT(v_short == i_short);
    EXPECT(v_ushort == i_ushort);
    EXPECT(v_long == i_long);
    EXPECT(v_ulong == i_ulong);
    EXPECT(v_longlong == i_longlong);
    EXPECT(v_ulonglong == i_ulonglong);
    //  EXPECT( v_float     ==     i_float );
    EXPECT(v_double == i_double);
    EXPECT(v_string == i_string);
    EXPECT(v_charp == i_charp);
}

CASE("stream_object") {
    Log::info() << "Stream an object" << std::endl;
    const std::string k("key");
    const std::string v("value");
    {
        FileStream sout(F::filename, "w");
        auto c = closer(sout);
        sout.startObject();
        sout << k;
        sout << v;
        sout.endObject();
    }
    {
        FileStream sin(F::filename, "r");
        auto c = closer(sin);
        EXPECT(sin.next());
        std::string s;
        sin >> s;
        EXPECT(s == k);
        sin >> s;
        EXPECT(s == v);
        EXPECT(sin.endObjectFound());
        EXPECT(!sin.next());
    }
}

CASE("stream_string") {
    Log::info() << "Stream a string" << std::endl;
    {
        FileStream sout(F::filename, "w");
        auto c = closer(sout);
        sout << i_string;
    }
    {
        FileStream sin(F::filename, "r");
        auto c = closer(sin);
        std::string s;
        EXPECT(sin.next(s));
        EXPECT(s == i_string);
        EXPECT(!sin.next());
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
