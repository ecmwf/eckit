// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/Log.h"
#include "eckit/parser/CSVParser.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_csv_1") {
    istringstream in(
        R"CSV(1,2,3
4,5,6)CSV");
    CSVParser p(in, false);
    Value v = p.parse();
    std::cout << v << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_csv_2") {
    istringstream in(
        R"CSV(a,b,c
1,2,3
4,5,6)CSV");
    CSVParser p(in, true);
    Value v = p.parse();
    std::cout << v << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

// CASE( "test_eckit_parser_eof" ) {
//     istringstream in("");
//     CSVParser p(in);

//     EXPECT_THROWS_AS(p.next(), StreamParser::Error);
// }

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
