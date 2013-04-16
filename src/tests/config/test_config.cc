/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Log.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eclib/Types.h"
#include "eclib/runtime/Tool.h"

#include "eclib/config/Script.h"
#include "eclib/config/Compiler.h"

using namespace eckit;

class TestConfig : public Tool {
public:

    TestConfig(int argc,char **argv): Tool(argc,argv) {}

    ~TestConfig() {}

    virtual void run();

    void test_parse();
    void test_resource();
};

//-----------------------------------------------------------------------------

void TestConfig::test_parse()
{
    ostringstream code;
     
    code <<        
            " a = 1; "                            // assign digit
            " b = lolo; "                         // assign string
            " "
//            " [ if class in [ od , rd ] && stream = oper ]"   // condition with multiple statement
            " [ if class = od || rd && stream = oper ]"   // condition with multiple statement
            " {  "
            "   fdbRoot = \'/tmp/fdb\';  "        // paths
            " "
            "   c1 = \"4 x 4\"; "                 // double quote
            "   [ if date = today ] "             // nested branch
            "   {"
            "       d = 4; "
            "   }"                                // close after semi-colon
            "   c2 = '5 x 5'; "                   // single quote
            "   cc = lolo popo   gege  ;"         // string with spaces and not quotes 'lolo popo'
            "   e = '6'; " 
            "   e = '66'; "                       // override
            "   f : 'fofo'; "                     // assign with :
            " }"
            " "
            " [ if xxx = yyy ] { f = ignored; }"   // branch not visited
            " "
            " [ if xxx = yyy ] {} || { s = ss } "  // else branch visited
            " "
            " [ if class = od ] {} "               // empty branch
            " "
            " [ if class = od || rd ] { t = 22 }"  // double or in branch
            " "
            " [ if ] { h = here ; }"               // always true
            " "
            " g = go; "                         // isolated statement
            " "
            " { } "                             // empty block
            " "
            " { k = koko; } "                   // stand alone block
            " "
            " [ function foo ] { m = momo }"    // function definition
            " "
            " [ call foo ]"                     // function call
            " "
            " { j = jojo  } "                   // finish assignement with block
            " z1 = 11 \n"                       // finish assignement with \n
            " z2 = 22 #"                        // finish assignement with #
            ; 
     
    istringstream in(code.str());
    
    config::Compiler c(in);
    
    config::Script s(c);
    
//    s.print( std::cout );
    
    StringDict din;
    
    din["class"]    = "od";
    din["stream"]   = "oper";
    din["date"]     = "today";

    StringDict dout;
    
    s.execute(din,dout);
    
    for( StringDict::const_iterator i = dout.begin(); i != dout.end(); ++i )
        std::cout << i->first << " : " << i->second << std::endl;    
    
    ASSERT( dout["a"] == "1" );
    ASSERT( dout["b"] == "lolo" );
    ASSERT( dout["fdbRoot"] == "/tmp/fdb" );
    ASSERT( dout["c1"] == "4 x 4" );
    ASSERT( dout["d"] == "4" );
    ASSERT( dout["c2"] == "5 x 5" );
    ASSERT( dout["cc"] == "lolo popo   gege" );
    ASSERT( dout["e"] == "66" );
    ASSERT( dout["s"] == "ss" );
    ASSERT( dout["t"] == "22" );
    ASSERT( dout["h"] == "here" );
    ASSERT( dout["g"] == "go" );
    ASSERT( dout["k"] == "koko" );
    ASSERT( dout["j"] == "jojo" );
    ASSERT( dout["m"] == "momo" );
    ASSERT( dout["z1"] == "11" );
    ASSERT( dout["z2"] == "22" );
    
    
}

//-----------------------------------------------------------------------------

void TestConfig::run()
{
    test_parse();
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestConfig mytest(argc,argv);
    mytest.start();
    return 0;
}

