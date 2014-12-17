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

#include "eckit/types/Types.h"
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
	std::string test = "EzLPYjRkayhnTCv47SgoFV5MOqbGt6emNlD231JWIXUiBKfAupwc0rQ8xHsZd9";
//	std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	int count = test.size();

    // for(int j = 0; j < 100 ; j++)
    {
		// std::random_shuffle( test.begin(),test.end() );

        std::cout << test << std::endl;

        unlink("foo");
		BTree<char,int, 128> btree("foo");

        for(int i = 0; i < count;  i++)
        {
			std::cout << "[" << test[i] << "," << -int(i) << "]" << std::endl;
			btree.set(test[i],-int(i));
        }
		std::cout << std::endl;

		std::vector< std::pair<char,int> > res;
		btree.range(char(32),char(126), res);
		for(int i = 0; i < res.size();  ++i)
			std::cout << "[" << res[i].first << "," << res[i].second << "]" << std::endl;

		std::cout << std::endl;

		BOOST_CHECK_EQUAL( btree.count() , test.size() );

		std::cout << std::endl;
		btree.dump() ;
		std::cout << std::endl;

        for(int i = 0; i < count; i++)
        {
            int k;
			BOOST_CHECK( btree.get(test[i],k) );
            BOOST_CHECK_EQUAL( k , (-int(i)) );
        }
    }

    {
        unlink("bar");

		BTree<FixedString<80>,int,1024> btree("bar");
        string f("foo");

		btree.set(f, 22);
		btree.set("barbar", 44);
		btree.set("zoulou", 484);
		btree.set("ZOULOU", 484);

        for(int i = 0; i < count;  i++)
        {
            string s;
            s += test[i];
			btree.set(s,-int(i));
        }

//        btree.dump();
    }

    {

        unlink("bar");

		BTree< FixedString<80>, FixedString<256>, 2048 > btree("bar");
        string f("foo");


        for(int i = 0; i < count;  i++)
        {
            string s;
            s += test[i];
			btree.set(s,s);
        }


		btree.set(f,f);

        FixedString<256> z;
		btree.get(f, z);
        std::cout << string(z) << std::endl;

		btree.dump();

    }

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
