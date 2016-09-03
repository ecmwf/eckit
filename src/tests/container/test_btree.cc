/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE eckit_test_btree

#include "ecbuild/boost_test_framework.h"

#include "eckit/types/Types.h"
#include "eckit/container/BTree.h"
#include "eckit/os/Semaphore.h"
#include "eckit/types/FixedString.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

struct UDF
{
	static const size_t SZ = 32;

	FixedString< SZ > s_;

	UDF( char c ) : s_() { std::string s; s += c; s_ = s; }
	UDF( const std::string& s = std::string() ) : s_(s) {}

	operator std::string() const { return s_.asString(); }

	friend std::ostream& operator<<(std::ostream& s,const UDF& p)
	{
		s << p.s_; return s;
	}

};

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_container_btree )

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_int_int_locking )
{
    std::string test = "EzLPYjRkayhnTCv47SgoFV5MOqbGt6emNlD231JWIXUiBKfAupwc0rQ8xHsZd9";

    unlink("foo");

    BTree<int,int, 128, BTreeLock> btree("foo");

    for(std::string::size_type i = 0; i < test.size(); ++i)
    {
//		std::cout << "[" << test[i] << "," << -int(i) << "]" << std::endl;
        int k = int(test[i]);
        btree.set(k,-int(i));
    }

    BOOST_CHECK_EQUAL( btree.count() , test.size() );

    std::vector< std::pair<int,int> > res;
    btree.range(32,126, res);

    //	for(int i = 0; i < res.size();  ++i)
    //		std::cout << "[" << res[i].first << "," << res[i].second << "]" << std::endl;

    BOOST_CHECK_EQUAL( btree.count(), res.size() );

//	std::cout << std::endl;
//	btree.dump() ;
//	std::cout << std::endl;

    for(std::string::size_type i = 0; i < test.size(); i++)
    {
        int k;
        BOOST_CHECK( btree.get(test[i],k) );
        BOOST_CHECK_EQUAL( k , (-int(i)) );
    }
}

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_int_int_nolocking )
{
    std::string test = "EzLPYjRkayhnTCv47SgoFV5MOqbGt6emNlD231JWIXUiBKfAupwc0rQ8xHsZd9";

    unlink("foo");

    BTree<int,int, 128, BTreeNoLock> btree("foo");

    for(std::string::size_type i = 0; i < test.size(); ++i)
    {
//		std::cout << "[" << test[i] << "," << -int(i) << "]" << std::endl;
        int k = int(test[i]);
        btree.set(k,-int(i));
    }

    BOOST_CHECK_EQUAL( btree.count() , test.size() );

    std::vector< std::pair<int,int> > res;
    btree.range(32,126, res);

    //	for(int i = 0; i < res.size();  ++i)
    //		std::cout << "[" << res[i].first << "," << res[i].second << "]" << std::endl;

    BOOST_CHECK_EQUAL( btree.count(), res.size() );

//	std::cout << std::endl;
//	btree.dump() ;
//	std::cout << std::endl;

    for(std::string::size_type i = 0; i < test.size(); i++)
    {
        int k;
        BOOST_CHECK( btree.get(test[i],k) );
        BOOST_CHECK_EQUAL( k , (-int(i)) );
    }
}

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_random_char_udf )
{
	std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for(int j = 0; j < 1 ; ++j)
	{
		std::random_shuffle( test.begin(),test.end() );

		unlink("foo");

        BTree< char, UDF, 6*UDF::SZ, BTreeLock> btree("foo");

        for(std::string::size_type i = 0; i < test.size();  i++)
		{
			// std::cout << "[" << test[i] << "," << test[i] << "]" << std::endl;
			btree.set( test[i], test[i] );
		}

		BOOST_CHECK_EQUAL( btree.count(), test.size() );

		std::vector< std::pair<char,UDF> > res;
		btree.range(char(32),char(126), res);

		//		for(int i = 0; i < res.size();  ++i)
		//			std::cout << "[" << res[i].first << "," << res[i].second << "]" << std::endl;
		//		std::cout << std::endl;

		BOOST_CHECK_EQUAL( btree.count(), res.size() );

		std::cout << std::endl;
		btree.dump() ;
		std::cout << std::endl;

        for(std::string::size_type i = 0; i < test.size(); i++)
		{
			UDF k;
			BOOST_CHECK( btree.get(test[i],k) );
			std::string s; s += test[i];
			BOOST_CHECK_EQUAL( (std::string)(k) , s );
		}
	}
}

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_random_char_fixedstring )
{
	std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	{
//		std::random_shuffle( test.begin(),test.end() );

		unlink("foo");

        BTree< char, FixedString<16>, 156, BTreeLock> btree("foo");

        for(std::string::size_type i = 0; i < test.size();  i++)
		{
			// std::cout << "[" << test[i] << "," << test[i] << "]" << std::endl;
			std::string s ("foo"); s +=test[i];
			btree.set( test[i], s );
		}

		BOOST_CHECK_EQUAL( btree.count(), test.size() );

        std::vector< std::pair<char,FixedString<16> > > res;
		btree.range(char(32),char(126), res);

		//		for(int i = 0; i < res.size();  ++i)
		//			std::cout << "[" << res[i].first << "," << res[i].second << "]" << std::endl;
		//		std::cout << std::endl;

		BOOST_CHECK_EQUAL( btree.count(), res.size() );

		std::cout << std::endl;
		btree.dump() ;
		std::cout << std::endl;

        for(std::string::size_type i = 0; i < test.size(); i++)
		{
			FixedString<16> v;
			BOOST_CHECK( btree.get(test[i],v) );
			std::string s ("foo"); s +=test[i];
			BOOST_CHECK_EQUAL( s , v.asString() );
		}
	}
}

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_random_char_int )
{
	std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	for(int j = 0; j < 100 ; j++)
	{
		std::random_shuffle( test.begin(),test.end() );

		unlink("foo");

        BTree<char,int, 128, BTreeLock> btree("foo");

        for(std::string::size_type i = 0; i < test.size();  i++)
		{
			// std::cout << "[" << test[i] << "," << -int(i) << "]" << std::endl;
			btree.set(test[i],-int(i));
		}

		BOOST_CHECK_EQUAL( btree.count(), test.size() );

		std::vector< std::pair<char,int> > res;
		btree.range(char(32),char(126), res);

		//		for(int i = 0; i < res.size();  ++i)
		//			std::cout << "[" << res[i].first << "," << res[i].second << "]" << std::endl;
		//		std::cout << std::endl;

		BOOST_CHECK_EQUAL( btree.count(), res.size() );

//		std::cout << std::endl;
//		btree.dump() ;
//		std::cout << std::endl;

        for(std::string::size_type i = 0; i < test.size(); i++)
		{
			int k;
			BOOST_CHECK( btree.get(test[i],k) );
			BOOST_CHECK_EQUAL( k , (-int(i)) );
		}
	}
}

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_fixedsttring_int_nolocking )
{
    std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    unlink("bar");

    BTree<FixedString<80>,int,1024, BTreeNoLock> btree("bar");

    string f("foo");

    btree.set(f, 22);
    btree.set("barbar", 44);
    btree.set("zoulou", 484);
    btree.set("ZOULOU", 484);

    for(std::string::size_type i = 0; i < test.size(); ++i)
    {
        string s;
        s += test[i];
        btree.set(s,-int(i));
    }

    int k;

    BOOST_CHECK( btree.get("foo",k) );
    BOOST_CHECK_EQUAL( k, 22 );

    BOOST_CHECK( btree.get("barbar",k) );
    BOOST_CHECK_EQUAL( k, 44 );

    BOOST_CHECK( btree.get("zoulou",k) );
    BOOST_CHECK_EQUAL( k, 484 );

    BOOST_CHECK( btree.get("ZOULOU",k) );
    BOOST_CHECK_EQUAL( k, 484 );
}

BOOST_AUTO_TEST_CASE( test_eckit_container_btree_fixedsttring_int_locking )
{
    std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    unlink("bar");

    BTree<FixedString<80>,int,1024, BTreeLock> btree("bar");

    string f("foo");

    btree.set(f, 22);
    btree.set("barbar", 44);
    btree.set("zoulou", 484);
    btree.set("ZOULOU", 484);

    for(std::string::size_type i = 0; i < test.size(); ++i)
    {
        string s;
        s += test[i];
        btree.set(s,-int(i));
    }

    int k;

    BOOST_CHECK( btree.get("foo",k) );
    BOOST_CHECK_EQUAL( k, 22 );

    BOOST_CHECK( btree.get("barbar",k) );
    BOOST_CHECK_EQUAL( k, 44 );

    BOOST_CHECK( btree.get("zoulou",k) );
    BOOST_CHECK_EQUAL( k, 484 );

    BOOST_CHECK( btree.get("ZOULOU",k) );
    BOOST_CHECK_EQUAL( k, 484 );
}

BOOST_AUTO_TEST_CASE( test_eckit_container_fixedsttring_fixedsttring )
{
	std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	unlink("bar");

    BTree< FixedString<80>, FixedString<256>, 2048, BTreeLock> btree("bar");

    for(std::string::size_type i = 0; i < test.size(); ++i)
	{
		string s;
		s += test[i];
		btree.set(s,s);
	}

	string f("foo");
	btree.set(f,f);

    for(std::string::size_type i = 0; i < test.size(); ++i)
	{
		string s;
		s += test[i];
		s += std::string("_bar");
		btree.set(s,s);
	}

	FixedString<256> z;

	BOOST_CHECK( btree.get(f, z) );
	BOOST_CHECK_EQUAL( z, "foo" );

	//	btree.dump();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
