/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eclib/Log.h"
#include "eclib/Tool.h"
#include "eclib/Resource.h"
#include "eclib/ResourceMgr.h"
#include "eclib/Types.h"

class TestCache : public Tool {
public:

    TestCache(int argc,char **argv): Tool(argc,argv) {}

    ~TestCache() {}

    virtual void run();

    void test_empty();

};

//-----------------------------------------------------------------------------

void TestCache::test_empty()
{
}

//-----------------------------------------------------------------------------
            
void TestCache::run()
{
    test_empty();
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestCache mytest(argc,argv);
    mytest.start();
    return 0;
}

