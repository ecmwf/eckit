/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#define BOOST_TEST_MODULE test_eckit_container

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/container/DenseMap.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_container_densemap )

BOOST_AUTO_TEST_CASE( test_eckit_container_densemap_0 )
{

   DenseMap<std::string,int> m;

   BOOST_CHECK_NO_THROW( m.insert( "two",  2 ) );
   BOOST_CHECK_NO_THROW( m.insert( "four", 4 ) );
   BOOST_CHECK_NO_THROW( m.insert( "nine", 9 ) );

   BOOST_CHECK_EQUAL( m.size(), 3 );

}

BOOST_AUTO_TEST_CASE( test_map_string_int )
{
    DenseMap<std::string,int> m;

    //

    m.insert( "two",  2 );

	BOOST_CHECK( !m.sorted() );

	BOOST_CHECK( m.size() == 1 );

    m.sort();

	BOOST_CHECK( m.sorted() );

	BOOST_CHECK( m.get("two") == 2 );

    //

    m.insert( "four", 4 );
    m.insert( "nine", 9 );

	BOOST_CHECK( !m.sorted() );

    m.sort();

	BOOST_CHECK( m.sorted() );

	BOOST_CHECK( m.size() == 3 );

	BOOST_CHECK( m.get("two") == 2 );
	BOOST_CHECK( m.get("four") == 4 );
	BOOST_CHECK( m.get("nine") == 9 );

    // failed find

	BOOST_CHECK( ! m.has("one") );

    // replace an existing value

    m.replace( "four", 44 );  
    m.replace( "nine", 99 );

	BOOST_CHECK( m.sorted() ); // still sorted

	BOOST_CHECK( m.size() == 3 );

	BOOST_CHECK( m.get("two") == 2 );
	BOOST_CHECK( m.get("four") == 44 );
	BOOST_CHECK( m.get("nine") == 99 );

    // replace into non-existing

	BOOST_CHECK( m.size() == 3 );

    m.replace( "five", 555 );  

	BOOST_CHECK( !m.sorted() );
    m.sort();

	BOOST_CHECK( m.get("two") == 2 );
	BOOST_CHECK( m.get("four") == 44 );
	BOOST_CHECK( m.get("nine") == 99 );
	BOOST_CHECK( m.get("five") == 555 );

	BOOST_CHECK( m.size() == 4 );

    //
    // std::cout << m << std::endl;

}

BOOST_AUTO_TEST_CASE( test_map_int_string )
{
    DenseMap<unsigned,std::string> m;

    //

    m.insert( 2, "two" );

	BOOST_CHECK( !m.sorted() );

	BOOST_CHECK( m.size() == 1 );

    m.sort();

	BOOST_CHECK( m.sorted() );

	BOOST_CHECK( m.get(2) == "two" );

    //

    m.insert( 4, "four" );
    m.insert( 9, "nine" );

	BOOST_CHECK( !m.sorted() );

    m.sort();

	BOOST_CHECK( m.sorted() );

	BOOST_CHECK( m.size() == 3 );

	BOOST_CHECK( m.get(2) == "two" );
	BOOST_CHECK( m.get(4) == "four" );
	BOOST_CHECK( m.get(9) == "nine" );

    // failed find

	BOOST_CHECK( m.find(1) == m.end() );

    // replace an existing value

    m.replace( 4, "FOUR" );  
    m.replace( 9, "NINE" );

	BOOST_CHECK( m.sorted() ); // still sorted

	BOOST_CHECK( m.size() == 3 );

	BOOST_CHECK( m.get(2) == "two" );
	BOOST_CHECK( m.get(4) == "FOUR" );
	BOOST_CHECK( m.get(9) == "NINE" );

    // replace into non-existing

	BOOST_CHECK( m.size() == 3 );

    m.replace( 5, "five" );  

	BOOST_CHECK( !m.sorted() );
    m.sort();

	BOOST_CHECK( m.get(2) == "two" );
	BOOST_CHECK( m.get(5) == "five" );
	BOOST_CHECK( m.get(4) == "FOUR" );
	BOOST_CHECK( m.get(9) == "NINE" );

	BOOST_CHECK( m.size() == 4 );

    // std::cout << m << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

