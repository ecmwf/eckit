/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_parser_stream

#include "ecbuild/boost_test_framework.h"

#include "eckit/parser/StreamParser.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_parser_stream )

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_stream_next ) {
    std::stringstream ss;
    ss << "1234567890abcdefgh";

    eckit::StreamParser parser(ss);

    BOOST_CHECK_EQUAL(parser.next(), '1');
    BOOST_CHECK_EQUAL(parser.next(), '2');
    for (int i = 0; i < 15; i++)
        parser.next();
    BOOST_CHECK_EQUAL(parser.next(), 'h');
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_stream_next_spaces ) {
    std::stringstream ss;
    ss << "1 3  6    ab def h";

    eckit::StreamParser parser(ss);

    BOOST_CHECK_EQUAL(parser.next(), '1');
    BOOST_CHECK_EQUAL(parser.next(), '3');  // n.b. by default spaces=false
    BOOST_CHECK_EQUAL(parser.next(true), ' ');
    BOOST_CHECK_EQUAL(parser.next(true), ' ');
    BOOST_CHECK_EQUAL(parser.next(true), '6');
    BOOST_CHECK_EQUAL(parser.next(true), ' ');
    BOOST_CHECK_EQUAL(parser.next(false), 'a');
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_stream_next_eof ) {
    // cf. commit: 7224cc431

    std::stringstream ss;
    ss << "12";

    eckit::StreamParser parser(ss);

    BOOST_CHECK_EQUAL(parser.next(), '1');
    BOOST_CHECK_EQUAL(parser.next(), '2');
    BOOST_CHECK_THROW(parser.next(), StreamParser::Error);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_stream_peek ) {
    std::stringstream ss;
    ss << " 2   5";

    eckit::StreamParser parser(ss);

    BOOST_CHECK_EQUAL(parser.peek(), '2'); // n.b. by default spaces=false
    BOOST_CHECK_EQUAL(parser.peek(), '2');
    BOOST_CHECK_EQUAL(parser.next(), '2');
    BOOST_CHECK_EQUAL(parser.peek(true), ' ');
    BOOST_CHECK_EQUAL(parser.peek(false), '5');
    BOOST_CHECK_EQUAL(parser.peek(true), '5');
    BOOST_CHECK_EQUAL(parser.next(true), '5');

    // And check eof behaviour
    BOOST_CHECK_EQUAL(parser.peek(), 0);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_stream_consume_char ) {
    std::stringstream ss;
    ss << " 2  567 ";

    eckit::StreamParser parser(ss);

    // n.b. by default the StreamParser ignores spaces.
    parser.consume('2');
    parser.consume('5');

    // Check that it throws on mismatch
    BOOST_CHECK_THROW(parser.consume('7'), StreamParser::Error); // 6 != 7
    parser.consume('7');

    // Check that it throws on eof
    BOOST_CHECK_THROW(parser.consume('8'), StreamParser::Error);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_stream_consume_string ) {
    std::stringstream ss;
    ss << " 234  789 0a";

    eckit::StreamParser parser(ss);

    // n.b. by default the StreamParser ignores spaces.
    parser.consume("234");

    // Check that it throws on mismatched contents
    BOOST_CHECK_THROW(parser.consume("779"), StreamParser::Error);

    // Check that it throws on eof
    BOOST_CHECK_THROW(parser.consume("0ab"), StreamParser::Error);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
