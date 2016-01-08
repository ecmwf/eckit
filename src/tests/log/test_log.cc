/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/runtime/Tool.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestLog : public Tool {
public:

    TestLog(int argc,char **argv): Tool(argc,argv) {}

    virtual void run();

    void test_debug();
    void test_info();
    void test_warning();
    void test_error();
    void test_panic();
    void test_strerr();
};


void TestLog::test_debug()
{
    Log::debug()         << "debug message 1" << std::endl;
    Log::debug( Here() ) << "debug message 2" << std::endl;
}

void TestLog::test_info()
{
    Log::info()         << "info message 1" << std::endl;
    Log::info( Here() ) << "info message 2" << std::endl;
}

void TestLog::test_warning()
{
    Log::warning()         << "warning message 1" << std::endl;
    Log::warning( Here() ) << "warning message 2" << std::endl;
}

void TestLog::test_error()
{
    Log::error()         << "error message 1" << std::endl;
    Log::error( Here() ) << "error message 2" << std::endl;
}

void TestLog::test_panic()
{
    Log::panic()         << "panic message 1" << std::endl;
    Log::panic( Here() ) << "panic message 2" << std::endl;
}

void TestLog::test_strerr()
{
    LocalPathName p("/tmp/edfpmjq3480hfnsribnzasdfibv");
    p.unlink();
}

void TestLog::run()
{
    test_debug();
    test_info();
    test_warning();
    test_error();
    test_panic();
    test_strerr();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestLog app(argc,argv);
    return app.start();
}

