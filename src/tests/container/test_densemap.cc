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

// #define BOOST_TEST_MODULE test_eckit_container
// #include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/container/DenseMap.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

// BOOST_AUTO_TEST_SUITE( test_eckit_container_densemap )

// BOOST_AUTO_TEST_CASE( test_eckit_container_densemap_0 )
// {

//   DenseMap<std::string,int> m;

//   BOOST_CHECK_NO_THROW( m.insert( "two",  2 ) );
//   BOOST_CHECK_NO_THROW( m.insert( "four", 4 ) );
//   BOOST_CHECK_NO_THROW( m.insert( "nine", 9 ) );

//   BOOST_CHECK_EQUAL( m.size(), 3 );

// }

// BOOST_AUTO_TEST_SUITE_END()

void test_map_string_int()
{
    DenseMap<std::string,int> m;

    //

    m.insert( "two",  2 );

    ASSERT( !m.sorted() );

    ASSERT( m.size() == 1 );

    m.sort();

    ASSERT( m.sorted() );

    ASSERT( m.get("two") == 2 );

    //

    m.insert( "four", 4 );
    m.insert( "nine", 9 );

    ASSERT( !m.sorted() );

    m.sort();

    ASSERT( m.sorted() );

    ASSERT( m.size() == 3 );

    ASSERT( m.get("two") == 2 );
    ASSERT( m.get("four") == 4 );
    ASSERT( m.get("nine") == 9 );

    // failed find

    ASSERT( ! m.has("one") );

    // replace an existing value

    m.replace( "four", 44 );  
    m.replace( "nine", 99 );

    ASSERT( m.sorted() ); // still sorted

    ASSERT( m.size() == 3 );

    ASSERT( m.get("two") == 2 );
    ASSERT( m.get("four") == 44 );
    ASSERT( m.get("nine") == 99 );

    // replace into non-existing

    ASSERT( m.size() == 3 );

    m.replace( "five", 555 );  

    ASSERT( !m.sorted() );
    m.sort();

    ASSERT( m.get("two") == 2 );
    ASSERT( m.get("four") == 44 );
    ASSERT( m.get("nine") == 99 );
    ASSERT( m.get("five") == 555 );

    ASSERT( m.size() == 4 );

    //
    // std::cout << m << std::endl;

}

void test_map_int_string()
{
    DenseMap<unsigned,std::string> m;

    //

    m.insert( 2, "two" );

    ASSERT( !m.sorted() );

    ASSERT( m.size() == 1 );

    m.sort();

    ASSERT( m.sorted() );

    ASSERT( m.get(2) == "two" );

    //

    m.insert( 4, "four" );
    m.insert( 9, "nine" );

    ASSERT( !m.sorted() );

    m.sort();

    ASSERT( m.sorted() );

    ASSERT( m.size() == 3 );

    ASSERT( m.get(2) == "two" );
    ASSERT( m.get(4) == "four" );
    ASSERT( m.get(9) == "nine" );

    // failed find

    ASSERT( m.find(1) == m.end() );

    // replace an existing value

    m.replace( 4, "FOUR" );  
    m.replace( 9, "NINE" );

    ASSERT( m.sorted() ); // still sorted

    ASSERT( m.size() == 3 );

    ASSERT( m.get(2) == "two" );
    ASSERT( m.get(4) == "FOUR" );
    ASSERT( m.get(9) == "NINE" );

    // replace into non-existing

    ASSERT( m.size() == 3 );

    m.replace( 5, "five" );  

    ASSERT( !m.sorted() );
    m.sort();

    ASSERT( m.get(2) == "two" );
    ASSERT( m.get(5) == "five" );
    ASSERT( m.get(4) == "FOUR" );
    ASSERT( m.get(9) == "NINE" );

    ASSERT( m.size() == 4 );

    // std::cout << m << std::endl;
}

int main()
{
    test_map_string_int();
    test_map_int_string();
}
