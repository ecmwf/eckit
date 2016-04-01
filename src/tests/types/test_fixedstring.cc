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

using namespace std;
using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_types_fixedstring )

//
// Start by testing the constructors
//


BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_zero )
{
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

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_default )
{
    FixedString<8> fs;

    BOOST_CHECK_EQUAL(fs.length(), 0);
    BOOST_CHECK_EQUAL(fs.asString(), "");
    BOOST_CHECK_EQUAL(std::string(fs), "");
}


BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_string )
{
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


BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_char )
{
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

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_constructor_fixedstring )
{
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

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_assignment_string )
{
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

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_assignment_fixedstring )
{
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

BOOST_AUTO_TEST_SUITE_END()
