/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/AIOHandle.h"
#include "eclib/Log.h"
#include "eclib/Tool.h"
#include "eclib/Types.h"

class TestAIOHandle : public Tool {
public:

    TestAIOHandle(int argc,char **argv): Tool(argc,argv) {}

    ~TestAIOHandle() {}

    virtual void run();
    
    void test_1();

};

//-----------------------------------------------------------------------------
            
void TestAIOHandle::test_1()
{
}

//-----------------------------------------------------------------------------
            
void TestAIOHandle::run()
{
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestAIOHandle mytest(argc,argv);
    mytest.start();
    return 0;
}

