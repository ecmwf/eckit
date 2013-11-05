/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/runtime/Context.h"

#include "eckit/mpi/Comm.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestMPIComm : public Tool {
public:

    TestMPIComm(int argc,char **argv): Tool(argc,argv) {}

    ~TestMPIComm() {}

    virtual void run();

    void setup();

    void test_helloworld();

    void teardown();
};

//-----------------------------------------------------------------------------

void TestMPIComm::setup()
{
    mpi::Comm::instance().init( Context::instance().argc(),
                                Context::instance().argvs() );
}

//-----------------------------------------------------------------------------

void TestMPIComm::teardown()
{
    mpi::Comm::instance().finalize();
}

//-----------------------------------------------------------------------------

void TestMPIComm::test_helloworld()
{
    mpi::Comm& comm = mpi::Comm::instance();

    ASSERT( comm.isInitialised() );
    ASSERT( comm.isActive() );

    Log::info() << "Hello World from process "
                << comm.rank()
                << " of "
                << comm.size() << std::endl;
}

//-----------------------------------------------------------------------------

void TestMPIComm::run()
{
    setup();

    test_helloworld();

    teardown();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestMPIComm mytest(argc,argv);
    mytest.start();
    return 0;
}

