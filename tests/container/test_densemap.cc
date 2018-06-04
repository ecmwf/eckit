/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/container/DenseMap.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

CASE( "test_eckit_container_densemap_0" ) {

   DenseMap<std::string, int> m;

   EXPECT_NO_THROW( m.insert( "two",  2 ) );
   EXPECT_NO_THROW( m.insert( "four", 4 ) );
   EXPECT_NO_THROW( m.insert( "nine", 9 ) );

   EXPECT( m.size() == 3 );
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "test_map_string_int" ) {
    DenseMap<std::string, int> m;

    //

    m.insert( "two",  2 );

    EXPECT( !m.sorted() );

    EXPECT( m.size() == 1 );

    m.sort();

    EXPECT( m.sorted() );

    EXPECT( m.get("two") == 2 );

    //

    m.insert( "four", 4 );
    m.insert( "nine", 9 );

    EXPECT( !m.sorted() );

    m.sort();

    EXPECT( m.sorted() );

    EXPECT( m.size() == 3 );

    EXPECT( m.get("two") == 2 );
    EXPECT( m.get("four") == 4 );
    EXPECT( m.get("nine") == 9 );

    // failed find

    EXPECT( !m.has("one") );

    // replace an existing value

    m.replace( "four", 44 );
    m.replace( "nine", 99 );

    EXPECT( m.sorted() );  // still sorted

    EXPECT( m.size() == 3 );

    EXPECT( m.get("two") == 2 );
    EXPECT( m.get("four") == 44 );
    EXPECT( m.get("nine") == 99 );

    // replace into non-existing

    EXPECT( m.size() == 3 );

    m.replace( "five", 555 );

    EXPECT( !m.sorted() );
    m.sort();

    EXPECT( m.get("two") == 2 );
    EXPECT( m.get("four") == 44 );
    EXPECT( m.get("nine") == 99 );
    EXPECT( m.get("five") == 555 );

    EXPECT( m.size() == 4 );

    //
    // std::cout << m << std::endl;

}

//----------------------------------------------------------------------------------------------------------------------

CASE( "test_map_int_string" ) {
    DenseMap<int, std::string> m;

    //

    m.insert( 2, "two" );

    EXPECT( !m.sorted() );

    EXPECT( m.size() == 1 );

    m.sort();

    EXPECT( m.sorted() );

    EXPECT( m.get(2) == "two" );

    //

    m.insert( 4, "four" );
    m.insert( 9, "nine" );

    EXPECT( !m.sorted() );

    m.sort();

    EXPECT( m.sorted() );

    EXPECT( m.size() == 3 );

    EXPECT( m.get(2) == "two" );
    EXPECT( m.get(4) == "four" );
    EXPECT( m.get(9) == "nine" );

    // failed find

    EXPECT( m.find(1) == m.end() );

    // replace an existing value

    m.replace( 4, "FOUR" );
    m.replace( 9, "NINE" );

    EXPECT( m.sorted() );  // still sorted

    EXPECT( m.size() == 3 );

    EXPECT( m.get(2) == "two" );
    EXPECT( m.get(4) == "FOUR" );
    EXPECT( m.get(9) == "NINE" );

    // replace into non-existing

    EXPECT( m.size() == 3 );

    m.replace( 5, "five" );

    EXPECT( !m.sorted() );
    m.sort();

    EXPECT( m.get(2) == "two" );
    EXPECT( m.get(5) == "five" );
    EXPECT( m.get(4) == "FOUR" );
    EXPECT( m.get(9) == "NINE" );
    EXPECT( m.get(9) == "NINE" );

    EXPECT( m.size() == 4 );

    // std::cout << m << std::endl;
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    return run_tests ( argc, argv );
}


