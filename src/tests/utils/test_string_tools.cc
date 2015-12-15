/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/parser/StringTools.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestStringTools : public Tool {
public:

    TestStringTools(int argc,char **argv): Tool(argc,argv) {}

    ~TestStringTools() {}

    virtual void run();

protected:
    
    void test_substitute();
    void test_substituteVariables();
    void test_startsWith();
    void test_trim();
    void test_front_trim();
    void test_back_trim();

};

//-----------------------------------------------------------------------------
            
void TestStringTools::run()
{
    test_substitute();
    test_substituteVariables();
    test_startsWith();
    test_trim();
    test_front_trim();
    test_back_trim();
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

void TestStringTools::test_trim()
{
    string t1 ( "   lolo_test    " );
    ASSERT(  StringTools::trim(t1) == string("lolo_test") );

    string t2 ( "   lolo_test" );
    ASSERT(  StringTools::trim(t2) == string("lolo_test") );

    string t3 ( "lolo_test   " );
    ASSERT(  StringTools::trim(t3) == string("lolo_test") );

    string t4 ( "" );
    ASSERT(  StringTools::trim(t4) == string("") );

    string t5 ( "nothing_here" );
    ASSERT(  StringTools::trim(t5) == string("nothing_here") );

    string t6 ( "XXXXXXusefullXXXXX" );
    ASSERT(  StringTools::trim(t6,"X") == string("usefull") );

    string t7 ( "0000010" );
    ASSERT(  StringTools::trim(t7,"0") == string("1") );
}

void TestStringTools::test_front_trim()
{
    string t1 ( "   lolo_test    " );
    ASSERT(  StringTools::front_trim(t1) == string("lolo_test    ") );

    string t2 ( "   lolo_test" );
    ASSERT(  StringTools::front_trim(t2) == string("lolo_test") );

    string t3 ( "lolo_test   " );
    ASSERT(  StringTools::front_trim(t3) == string("lolo_test   ") );

    string t4 ( "" );
    ASSERT(  StringTools::front_trim(t4) == string("") );

    string t5 ( "nothing_here" );
    ASSERT(  StringTools::front_trim(t5) == string("nothing_here") );

    string t6 ( "XXXXXXusefullXXXXX" );
    ASSERT(  StringTools::front_trim(t6,"X") == string("usefullXXXXX") );

    string t7 ( "0000010" );
    ASSERT(  StringTools::front_trim(t7,"0") == string("10") );
}

void TestStringTools::test_back_trim()
{
    string t1 ( "   lolo_test    " );

    Log::info() << StringTools::back_trim(t1) << std::endl;

    ASSERT(  StringTools::back_trim(t1) == string("   lolo_test") );

    string t2 ( "   lolo_test" );
    ASSERT(  StringTools::back_trim(t2) == string("   lolo_test") );

    string t3 ( "lolo_test   " );
    ASSERT(  StringTools::back_trim(t3) == string("lolo_test") );

    string t4 ( "" );
    ASSERT(  StringTools::back_trim(t4) == string("") );

    string t5 ( "nothing_here" );
    ASSERT(  StringTools::back_trim(t5) == string("nothing_here") );

    string t6 ( "XXXXXXusefullXXXXX" );
    ASSERT(  StringTools::back_trim(t6,"X") == string("XXXXXXusefull") );

    string t7 ( "0000010" );
    ASSERT(  StringTools::back_trim(t7,"0") == string("000001") );
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestStringTools mytest(argc,argv);
    mytest.start();
    return 0;
}

