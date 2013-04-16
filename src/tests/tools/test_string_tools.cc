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
#include "eclib/StringTools.h"
#include "eclib/Types.h"

using namespace eckit;

class TestStringTools : public Tool {
public:

    TestStringTools(int argc,char **argv): Tool(argc,argv) {}

    ~TestStringTools() {}

    virtual void run();

protected:
    
    void test_substitute();
    void test_substituteVariables();
    void test_startsWith();    
    
};

//-----------------------------------------------------------------------------
            
void TestStringTools::run()
{
    test_substitute();
    test_substituteVariables();
    test_startsWith();
}

void TestStringTools::test_substitute()
{
    StringDict m;

    m["class"]  = "od";
    m["stream"] = "oper";

    string in ( "{class}:none:{stream}" );
    
    string out = StringTools::substitute(in,m);
    
    ASSERT( out == "od:none:oper" );    
}

void TestStringTools::test_substituteVariables()
{
    string in ( "{class}:none:{stream}" );

    StringList out = StringTools::substituteVariables(in);
    
    ASSERT( out.size() == 2 );    
    ASSERT( out[0] == "class" );    
    ASSERT( out[1] == "stream" );    
}

void TestStringTools::test_startsWith()
{
    string in ( "_lolo_test" );
    string s1 ( "_lolo" );
    string s2 ( "lolo" );
    string s3 ( "_lolo_test_bigger" );

    ASSERT( !StringTools::startsWith(in,"") );    
    ASSERT(  StringTools::startsWith(in,in) );    
    ASSERT(  StringTools::startsWith(in,s1) );    
    ASSERT(  StringTools::startsWith(in,"_") );    
    ASSERT( !StringTools::startsWith(in,s2) );    
    ASSERT( !StringTools::startsWith(in,s3) );    
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestStringTools mytest(argc,argv);
    mytest.start();
    return 0;
}

