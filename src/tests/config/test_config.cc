/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_config

#include "ecbuild/boost_test_framework.h"

#include "eckit/log/Log.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/types/Types.h"

#include "eckit/config/Script.h"
#include "eckit/config/Compiler.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_config )

BOOST_AUTO_TEST_CASE( test_parse )
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
    
    BOOST_CHECK( dout["a"] == "1" );
    BOOST_CHECK( dout["b"] == "lolo" );
    BOOST_CHECK( dout["fdbRoot"] == "/tmp/fdb" );
    BOOST_CHECK( dout["c1"] == "4 x 4" );
    BOOST_CHECK( dout["d"] == "4" );
    BOOST_CHECK( dout["c2"] == "5 x 5" );
    BOOST_CHECK( dout["cc"] == "lolo popo   gege" );
    BOOST_CHECK( dout["e"] == "66" );
    BOOST_CHECK( dout["s"] == "ss" );
    BOOST_CHECK( dout["t"] == "22" );
    BOOST_CHECK( dout["h"] == "here" );
    BOOST_CHECK( dout["g"] == "go" );
    BOOST_CHECK( dout["k"] == "koko" );
    BOOST_CHECK( dout["j"] == "jojo" );
    BOOST_CHECK( dout["m"] == "momo" );
    BOOST_CHECK( dout["z1"] == "11" );
    BOOST_CHECK( dout["z2"] == "22" );
    
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
