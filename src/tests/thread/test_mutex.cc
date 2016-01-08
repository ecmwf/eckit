/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/Mutex.h"
#include "eckit/runtime/Tool.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

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

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestMutex app(argc,argv);
    return app.execute();
}

