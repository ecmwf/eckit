/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/parser/CSVParser.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

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

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
