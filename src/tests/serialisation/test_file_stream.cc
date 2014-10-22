/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cstdio>

#include "eckit/runtime/Tool.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/log/Log.h"

using namespace std;
using namespace eckit;

class TestApp : public Tool {

public:

    TestApp( int argc, char** argv ) : Tool(argc,argv),
    v_char(0),
    v_uchar(0),
    v_bool(0),
    v_int(0),
    v_uint(0),
    v_short(0),
    v_ushort(0),
    v_long(0),
    v_ulong(0),
    v_longlong(0),
    v_ulonglong(0),
    v_double(0)
    {
        PathName filename = PathName::unique( "data." );
        filepath_ = filename.asString();
    }

private: // methods

    void run();

    void write_data();
    void read_data();
    void check_data();
    void remove_file();

private: // data members

    std::string filepath_;

    char             v_char;
    unsigned char    v_uchar;
    bool             v_bool;
    int              v_int;
    unsigned int     v_uint;
    short            v_short;
    unsigned short   v_ushort;
    long             v_long;
    unsigned long    v_ulong;
    long long          v_longlong;
    unsigned long long v_ulonglong;
//    float             v_float;
    double             v_double;
    string             v_string;
    string             v_charp;

};

void TestApp::run()
{
    write_data();
    read_data();
    check_data();
    remove_file();
}

void TestApp::remove_file()
{
//    if( ::unlink(filepath) == -1 )
//      perror("Error removing the file"), exit(EXIT_FAILURE);
}

void TestApp::write_data()
{
    FileStream sout( filepath_.c_str(), "w" );

    sout
      << char('a')
      << (unsigned char)('b')
      << bool(true)
      << int(-20000000)
      << (unsigned int)(30000000)
      << short(-4000)
      << (unsigned short)(5000)
      << long(60000000)
      << (unsigned long)(70000000)
      << (long long)(80000000)
      << (unsigned long long)(90000000)
//      << float(3)
      << double(100000000)
      << std::string("abcdefghijklmnopqrstuvwyxz")
      << "abcdefghijklmnopqrstuvwyxz"
    ;
}

void TestApp::read_data()
{
    FileStream sin( filepath_.c_str(), "r" );

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

void TestApp::check_data()
{

    Log::info()
        << v_char << std::endl
        << v_uchar << std::endl
        << v_bool << std::endl
        << v_int << std::endl
        << v_uint << std::endl
        << v_short << std::endl
        << v_ushort << std::endl
        << v_long << std::endl
        << v_ulong << std::endl
        << v_longlong << std::endl
        << v_ulonglong << std::endl
    //  << v_float << std::endl
        << v_double << std::endl
        << v_string << std::endl
        << v_charp << std::endl
    ;

}

int main(int argc,char **argv)
{
    TestApp app(argc,argv);
    app.start();
}
