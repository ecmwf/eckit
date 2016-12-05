/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_types

#include "ecbuild/boost_test_framework.h"

#include "eckit/types/FixedString.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_types_fixedstring )

//----------------------------------------------------------------------------------------------------------------------
//
// Start by testing the constructors
//
//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_zero ) {
    // What happens if we create a length-zero FixedString?

    FixedString<0> fs;

    BOOST_CHECK_EQUAL(fs.length(), 0);
    BOOST_CHECK_EQUAL(fs.size(), 0);
    BOOST_CHECK_EQUAL(fs.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs), "");

    FixedString<0> fs2("");
    BOOST_CHECK_EQUAL(fs2.length(), 0);
    BOOST_CHECK_EQUAL(fs2.size(), 0);
    BOOST_CHECK_EQUAL(fs2.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs2), "");
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_default ) {
    FixedString<8> fs;

    BOOST_CHECK_EQUAL(fs.length(), 0);
    BOOST_CHECK_EQUAL(fs.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs), "");
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_string ) {
    // Construct with a string of the correct length

    std::string str1("12345678");
    FixedString<8> fs1(str1);

    BOOST_CHECK_EQUAL(fs1.size(), 8);
    BOOST_CHECK_EQUAL(fs1.length(), 8);
    BOOST_CHECK_EQUAL(fs1.asString(), "12345678");
    BOOST_CHECK_EQUAL(std::string(fs1), "12345678");

    // Construct with a string that is too short

    std::string str2("1234");
    FixedString<8> fs2(str2);

    BOOST_CHECK_EQUAL(fs2.size(), 8);
    BOOST_CHECK_EQUAL(fs2.length(), 4);
    BOOST_CHECK_EQUAL(fs2.asString(), "1234");
    BOOST_CHECK_EQUAL(std::string(fs2), "1234");

    // Construct with a string that is too long

    // n.b. use functor to make exception thrown in constructor palatable to BOOST_CHECK_THROW
    struct fs3_allocate {
        void operator()() {
            std::string str3("1234567890");
            FixedString<8> fs3(str3);
        }
    };
    BOOST_CHECK_THROW(fs3_allocate()(), AssertionFailed);

    // Construct with a zero length string

    std::string str4;
    FixedString<8> fs4(str4);

    BOOST_CHECK_EQUAL(fs4.size(), 8);
    BOOST_CHECK_EQUAL(fs4.length(), 0);
    BOOST_CHECK_EQUAL(fs4.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs4), "");
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_char ) {
    // Construct with a string of the correct length

    FixedString<8> fs1("12345678");

    BOOST_CHECK_EQUAL(fs1.size(), 8);
    BOOST_CHECK_EQUAL(fs1.length(), 8);
    BOOST_CHECK_EQUAL(fs1.asString(), "12345678");
    BOOST_CHECK_EQUAL(std::string(fs1), "12345678");

    // Construct with a string that is too short

    FixedString<8> fs2("1234");

    BOOST_CHECK_EQUAL(fs2.size(), 8);
    BOOST_CHECK_EQUAL(fs2.length(), 4);
    BOOST_CHECK_EQUAL(fs2.asString(), "1234");
    BOOST_CHECK_EQUAL(std::string(fs2), "1234");

    // Construct with a string that is too long

    // n.b. use functor to make exception thrown in constructor palatable to BOOST_CHECK_THROW
    struct fs3_allocate {
        void operator()() {
            FixedString<8> fs3("1234567890");
        }
    };
    BOOST_CHECK_THROW(fs3_allocate()(), AssertionFailed);

    // Construct with a zero length string

    FixedString<8> fs4("");

    BOOST_CHECK_EQUAL(fs4.size(), 8);
    BOOST_CHECK_EQUAL(fs4.length(), 0);
    BOOST_CHECK_EQUAL(fs4.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs4), "");

    // Construct with a null pointer

    struct fs5_allocate {
        void operator()() {
            FixedString<8> fs5(0);
        }
    };
    BOOST_CHECK_THROW(fs5_allocate()(), AssertionFailed);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_fixedstring ) {
    // Construct with a string of the correct length

    /// @note for all of these assignments, except for the <8> --> <8> the std::string constructor is used. It might
    ///       be better for the long term if a template <int SIZE2> FixedString(const FixedString<SIZE2>& )
    ///       constructor existed.

    std::string str1("12345678");
    FixedString<8> fs1(str1);
    FixedString<8> fs1a(fs1);

    BOOST_CHECK_EQUAL(fs1a.size(), 8);
    BOOST_CHECK_EQUAL(fs1a.length(), 8);
    BOOST_CHECK_EQUAL(fs1a.asString(), "12345678");
    BOOST_CHECK_EQUAL(std::string(fs1a), "12345678");

    // Construct with a string that is too short

    std::string str2("1234");
    FixedString<4> fs2(str2);
    FixedString<8> fs2a(fs2);

    BOOST_CHECK_EQUAL(fs2a.size(), 8);
    BOOST_CHECK_EQUAL(fs2a.length(), 4);
    BOOST_CHECK_EQUAL(fs2a.asString(), "1234");
    BOOST_CHECK_EQUAL(std::string(fs2a), "1234");

    // Construct with a string that is too long

    // n.b. use functor to make exception thrown in constructor palatable to BOOST_CHECK_THROW
    struct fs3_allocate {
        void operator()() {
            std::string str3("1234567890");
            FixedString<10> fs3(str3);
            FixedString<8> fs3a(fs3);
        }
    };
    BOOST_CHECK_THROW(fs3_allocate()(), AssertionFailed);

    // Construct with a zero length string

    std::string str4;
    FixedString<0> fs4(str4);
    FixedString<8> fs4a(fs4);

    BOOST_CHECK_EQUAL(fs4a.size(), 8);
    BOOST_CHECK_EQUAL(fs4a.length(), 0);
    BOOST_CHECK_EQUAL(fs4a.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs4a), "");
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_assignment_string ) {
    // Construct with a string of the correct length

    FixedString<8> fs1 = std::string("12345678");

    BOOST_CHECK_EQUAL(fs1.size(), 8);
    BOOST_CHECK_EQUAL(fs1.length(), 8);
    BOOST_CHECK_EQUAL(fs1.asString(), "12345678");
    BOOST_CHECK_EQUAL(std::string(fs1), "12345678");

    // Construct with a string that is too short

    FixedString<8> fs2 = std::string("1234");

    BOOST_CHECK_EQUAL(fs2.size(), 8);
    BOOST_CHECK_EQUAL(fs2.length(), 4);
    BOOST_CHECK_EQUAL(fs2.asString(), "1234");
    BOOST_CHECK_EQUAL(std::string(fs2), "1234");

    // Construct with a string that is too long

    // n.b. use functor to make exception thrown in assignment palatable to BOOST_CHECK_THROW
    struct fs3_assign {
        void operator()() {
            FixedString<8> fs3 = std::string("1234567890");
            (void) fs3;
        }
    };
    BOOST_CHECK_THROW(fs3_assign()(), AssertionFailed);

    // Construct with a zero length string

    FixedString<8> fs4 = std::string();

    BOOST_CHECK_EQUAL(fs4.size(), 8);
    BOOST_CHECK_EQUAL(fs4.length(), 0);
    BOOST_CHECK_EQUAL(fs4.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs4), "");
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_assignment_fixedstring ) {
    // Construct with a string of the correct length

    /// @note for all of these assignments, except for the <8> --> <8> the std::string constructor is used. It might
    ///       be better for the long term if a template <int SIZE2> FixedString(const FixedString<SIZE2>& )
    ///       constructor existed.

    FixedString<8> fs1("12345678");
    FixedString<8> fs1a = fs1;

    BOOST_CHECK_EQUAL(fs1a.size(), 8);
    BOOST_CHECK_EQUAL(fs1a.length(), 8);
    BOOST_CHECK_EQUAL(fs1a.asString(), "12345678");
    BOOST_CHECK_EQUAL(std::string(fs1a), "12345678");

    /// Mismatched size assignments are (correctly) compile-time disallowed.

    /// // Construct with a string that is too short

    /// FixedString<4> fs2("1234");
    /// FixedString<8> fs2a = fs2;

    /// BOOST_CHECK_EQUAL(fs2a.size(), 8);
    /// BOOST_CHECK_EQUAL(fs2a.length(), 4);
    /// BOOST_CHECK_EQUAL(fs2a.asString(), "1234");
    /// BOOST_CHECK_EQUAL(std::string(fs2a), "1234");

    /// // Construct with a string that is too long

    /// // n.b. use functor to make exception thrown in constructor palatable to BOOST_CHECK_THROW
    /// struct fs3_allocate {
    ///     void operator()() {
    ///         FixedString<10> fs3("1234567890");
    ///         FixedString<8> fs3a = fs3;
    ///     }
    /// };
    /// BOOST_CHECK_THROW(fs3_allocate()(), AssertionFailed);

    /// // Construct with a zero length string

    /// FixedString<0> fs4;
    /// FixedString<8> fs4a = fs4;

    /// BOOST_CHECK_EQUAL(fs4a.size(), 8);
    /// BOOST_CHECK_EQUAL(fs4a.length(), 0);
    /// BOOST_CHECK_EQUAL(fs4a.asString(), "");
    /// BOOST_CHECK_EQUAL(std::string(fs4a), "");

    // What happens if we assign to ourself?

    fs1a = fs1a;

    BOOST_CHECK_EQUAL(fs1a.size(), 8);
    BOOST_CHECK_EQUAL(fs1a.length(), 8);
    BOOST_CHECK_EQUAL(fs1a.asString(), "12345678");
    BOOST_CHECK_EQUAL(std::string(fs1a), "12345678");
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test comparison operators
//
//----------------------------------------------------------------------------------------------------------------------

/// @note Comparisons between different FixedString<N>, FixedString<M> not permitted at compile time.

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_operators ) {
    FixedString<8> fs1("12345678");
    FixedString<8> fs2("12345678");
    FixedString<8> fs3("abcdefgh");

    // ==

    BOOST_CHECK(fs1 == fs1);
    BOOST_CHECK(fs1 == fs2);
    BOOST_CHECK(!(fs1 == fs3));

    // !=

    BOOST_CHECK(!(fs1 != fs1));
    BOOST_CHECK(!(fs1 != fs2));
    BOOST_CHECK(fs1 != fs3);

    // <

    BOOST_CHECK(!(fs1 < fs1));
    BOOST_CHECK(!(fs1 < fs2));
    BOOST_CHECK(fs1 < fs3);
    BOOST_CHECK(!(fs3 < fs1));

    // >

    BOOST_CHECK(!(fs1 > fs1));
    BOOST_CHECK(!(fs1 > fs2));
    BOOST_CHECK(!(fs1 > fs3));
    BOOST_CHECK(fs3 > fs1);

    // <=

    BOOST_CHECK(fs1 <= fs1);
    BOOST_CHECK(fs1 <= fs2);
    BOOST_CHECK(fs1 <= fs3);
    BOOST_CHECK(!(fs3 <= fs1));

    // >=

    BOOST_CHECK(fs1 >= fs1);
    BOOST_CHECK(fs1 >= fs2);
    BOOST_CHECK(!(fs1 >= fs3));
    BOOST_CHECK(fs3 >= fs1);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_operators_zero ) {
    FixedString<0> fs1;
    FixedString<0> fs2;

    // ==

    BOOST_CHECK(fs1 == fs1);
    BOOST_CHECK(fs1 == fs2);

    // !=

    BOOST_CHECK(!(fs1 != fs1));
    BOOST_CHECK(!(fs1 != fs2));

    // <

    BOOST_CHECK(!(fs1 < fs1));
    BOOST_CHECK(!(fs1 < fs2));

    // >

    BOOST_CHECK(!(fs1 > fs1));
    BOOST_CHECK(!(fs1 > fs2));

    // <=

    BOOST_CHECK(fs1 <= fs1);
    BOOST_CHECK(fs1 <= fs2);

    // >=

    BOOST_CHECK(fs1 >= fs1);
    BOOST_CHECK(fs1 >= fs2);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test extraneous bits
//
//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_byte_size ) {
    FixedString<0> fs0;
    FixedString<1> fs1;
    FixedString<4> fs4;
    FixedString<8> fs8;

    BOOST_CHECK_EQUAL(sizeof(fs0), 0);
    BOOST_CHECK_EQUAL(sizeof(fs1), 1);
    BOOST_CHECK_EQUAL(sizeof(fs4), 4);
    BOOST_CHECK_EQUAL(sizeof(fs8), 8);

    BOOST_CHECK_EQUAL(sizeof(FixedString<0>), 0);
    BOOST_CHECK_EQUAL(sizeof(FixedString<1>), 1);
    BOOST_CHECK_EQUAL(sizeof(FixedString<4>), 4);
    BOOST_CHECK_EQUAL(sizeof(FixedString<8>), 8);

    BOOST_CHECK_EQUAL(fs0.size(), 0);
    BOOST_CHECK_EQUAL(fs1.size(), 1);
    BOOST_CHECK_EQUAL(fs4.size(), 4);
    BOOST_CHECK_EQUAL(fs8.size(), 8);

    BOOST_CHECK_EQUAL(FixedString<0>::static_size(), 0);
    BOOST_CHECK_EQUAL(FixedString<1>::static_size(), 1);
    BOOST_CHECK_EQUAL(FixedString<4>::static_size(), 4);
    BOOST_CHECK_EQUAL(FixedString<8>::static_size(), 8);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_data_access ) {
    FixedString<8> fs("12345678");

    // Check that const and non-const pointers give access to same data...

    char* d = fs.data();
    const char* cd = fs.data();

    BOOST_CHECK_EQUAL(d, cd);

    // Check that if we insert a \0 character appropriately, the accessible strings and lengths adjust correctly

    BOOST_CHECK_EQUAL(std::string(fs), "12345678");
    BOOST_CHECK_EQUAL(fs.length(), 8);

    d[4] = '\0';
    BOOST_CHECK_EQUAL(std::string(fs), "1234");
    BOOST_CHECK_EQUAL(fs.length(), 4);

    d[6] = '\0';
    d[4] = 'F';
    BOOST_CHECK_EQUAL(std::string(fs), "1234F6");
    BOOST_CHECK_EQUAL(fs.length(), 6);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_reassign_shorter_string ) {

    ///< @see ECKIT-182

    FixedString<64> fs;

    fs = std::string("calvin");

    BOOST_CHECK_EQUAL(fs, std::string("calvin")); /* this worked */

    fs = std::string("calvin & hobbes");

    BOOST_CHECK_EQUAL(fs, std::string("calvin & hobbes")); /* assinging longer string also worked */

    fs = std::string("susie");

    BOOST_CHECK_EQUAL(fs, std::string("susie")); /* but then assigning shorter did not, resulted "susien & hobbes" */
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
