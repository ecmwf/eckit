/*
 * (C) Copyright 1996-2017 ECMWF.
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


namespace eckit_test {

//----------------------------------------------------------------------------------------------------------------------

class TestStringTools : public Tool {
public:

    TestStringTools(int argc,char **argv): Tool(argc,argv) {}

    ~TestStringTools() {}

    virtual void run();

protected:
    
    void substitute();
    void listVariables();
    void startsWith();
    void endsWith();
    void trim();
    void front_trim();
    void back_trim();

};


            
void TestStringTools::run()
{
    substitute();
    listVariables();
    startsWith();
    trim();
    front_trim();
    back_trim();
}

void TestStringTools::substitute()
{
    StringDict m;

    m["class"]  = "od";
    m["stream"] = "oper";

    string in ( "{class}:none:{stream}" );
    
    string out = StringTools::substitute(in,m);
    
    ASSERT( out == "od:none:oper" );    
}

void TestStringTools::listVariables()
{
    string in ( "{class}:none:{stream}" );

    StringList out = StringTools::listVariables(in);
    
    ASSERT( out.size() == 2 );    
    ASSERT( out[0] == "class" );    
    ASSERT( out[1] == "stream" );    
}

void TestStringTools::startsWith()
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

void TestStringTools::endsWith()
{
    string in ( "_lolo_test" );
    ASSERT(  StringTools::endsWith(in,"") );
    ASSERT(  StringTools::endsWith(in,in) );
    ASSERT(  StringTools::endsWith(in,"t") );

    string s1 ( "_test" );
    ASSERT(  StringTools::endsWith(in,s1) );

    string s2 ( "lolo" );
    ASSERT( !StringTools::endsWith(in,s2) );

    string s3 ( "_lolo_test333" );
    ASSERT( !StringTools::endsWith(in,s3) );
}

void TestStringTools::trim()
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

void TestStringTools::front_trim()
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

void TestStringTools::back_trim()
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

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit_test


int main(int argc,char **argv)
{
    eckit_test::TestStringTools app(argc,argv);
    return app.start();
}

