/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ecbuild/boost_test_framework.h"

#include "eckit/container/BTree.h"
#include "eckit/os/Semaphore.h"
#include "eckit/types/FixedString.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_container_btree )

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_constructor )
{
    char test[] = "EzLPYjRkayhnTCv47SgoFV5MOqbGt6emNlD231JWIXUiBKfAupwc0rQ8xHsZd9\0";
    // char test[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\0";

    int count = strlen(test);

    // for(int j = 0; j < 100 ; j++)
    {
        // std::random_shuffle(test,test+count);

        std::cout << test << std::endl;

        unlink("foo");
        BTree<char,int, 128> b("foo");

        for(int i = 0; i < count;  i++)
        {
            std::cout << "[" << test[i] << "," << -int(i) << "]" << std::endl;
            b.set(test[i],-int(i));
        }

       std::cout << std::endl;
       b.dump();
       std::cout << std::endl;
       
        for(int i = 0; i < count; i++)
        {
            int k;
            BOOST_CHECK( b.get(test[i],k) );
            BOOST_CHECK_EQUAL( k , (-int(i)) );
        }
    }

    {
        unlink("bar");

        BTree<FixedString<80>,int,1024> b("bar");
        string f("foo");

        b.set(f, 22);
        b.set("barbar", 44);
        b.set("zoulou", 484);
        b.set("ZOULOU", 484);

        for(int i = 0; i < count;  i++)
        {
            string s;
            s += test[i];
            b.set(s,-int(i));
        }

//        b.dump();
    }


    if(1)
    {

        unlink("bar");

        BTree< FixedString<80>, FixedString<256>, 2048 > b("bar");
        string f("foo");


        for(int i = 0; i < count;  i++)
        {
            string s;
            s += test[i];
            b.set(s,s);
        }


        b.set(f,f);

        FixedString<256> z;
        b.get(f, z);
        std::cout << string(z) << std::endl;

        b.dump();

    }

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
