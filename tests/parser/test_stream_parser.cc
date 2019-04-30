/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/parser/StreamParser.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_stream_next") {
    std::stringstream ss;
    ss << "1234567890abcdefgh";

    eckit::StreamParser parser(ss);

    EXPECT(parser.next() == '1');
    EXPECT(parser.next() == '2');
    for (int i = 0; i < 15; i++)
        parser.next();
    EXPECT(parser.next() == 'h');
}

CASE("test_eckit_parser_stream_next_spaces") {
    std::stringstream ss;
    ss << "1 3  6    ab def h";

    eckit::StreamParser parser(ss);

    EXPECT(parser.next() == '1');
    EXPECT(parser.next() == '3');  // n.b. by default spaces=false
    EXPECT(parser.next(true) == ' ');
    EXPECT(parser.next(true) == ' ');
    EXPECT(parser.next(true) == '6');
    EXPECT(parser.next(true) == ' ');
    EXPECT(parser.next(false) == 'a');
}

CASE("test_eckit_parser_stream_next_eof") {
    // cf. commit: 7224cc431

    std::stringstream ss;
    ss << "12";

    eckit::StreamParser parser(ss);

    EXPECT(parser.next() == '1');
    EXPECT(parser.next() == '2');
    EXPECT_THROWS_AS(parser.next(), StreamParser::Error);
}

CASE("test_eckit_parser_stream_peek") {
    std::stringstream ss;
    ss << " 2   5";

    eckit::StreamParser parser(ss);

    EXPECT(parser.peek() == '2');  // n.b. by default spaces=false
    EXPECT(parser.peek() == '2');
    EXPECT(parser.next() == '2');
    EXPECT(parser.peek(true) == ' ');
    EXPECT(parser.peek(false) == '5');
    EXPECT(parser.peek(true) == '5');
    EXPECT(parser.next(true) == '5');

    // And check eof behaviour
    EXPECT(parser.peek() == 0);
}

CASE("test_eckit_parser_stream_consume_char") {
    std::stringstream ss;
    ss << " 2  567 ";

    eckit::StreamParser parser(ss);

    // n.b. by default the StreamParser ignores spaces.
    parser.consume('2');
    parser.consume('5');

    // Check that it throws on mismatch
    EXPECT_THROWS_AS(parser.consume('7'), StreamParser::Error);  // 6 != 7
    parser.consume('7');

    // Check that it throws on eof
    EXPECT_THROWS_AS(parser.consume('8'), StreamParser::Error);
}

CASE("test_eckit_parser_stream_consume_string") {
    std::stringstream ss;
    ss << " 234  789 0a";

    eckit::StreamParser parser(ss);

    // n.b. by default the StreamParser ignores spaces.
    parser.consume("234");

    // Check that it throws on mismatched contents
    EXPECT_THROWS_AS(parser.consume("779"), StreamParser::Error);

    // Check that it throws on eof
    EXPECT_THROWS_AS(parser.consume("0ab"), StreamParser::Error);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
