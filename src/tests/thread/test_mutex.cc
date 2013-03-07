/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Mutex.h"
#include "eclib/runtime/Tool.h"

using namespace eckit;

class TestMutex : public Tool {
public:

    TestMutex(int argc,char **argv): Tool(argc,argv) {}

    ~TestMutex() {}

    virtual void run();

protected:
    
    void test_constructor();
    
};

//-----------------------------------------------------------------------------

void TestMutex::test_constructor()
{
    Mutex* m = new Mutex();
    
    m->lock();
    
    m->unlock();
    
    delete m;   
}

//-----------------------------------------------------------------------------
            
void TestMutex::run()
{
    test_constructor();
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestMutex mytest(argc,argv);
    mytest.start();
    return 0;
}

