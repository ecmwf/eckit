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
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_print_vector_string") {
    vector<string> vstr;
    vstr.push_back("Hello");
    vstr.push_back("World");
    vstr.push_back("test");
    vstr.push_back("case");

    stringstream s;
    s << vstr;

    EXPECT("[Hello,World,test,case]" == s.str());
}

CASE("test_eckit_print_vector_pair") {
    vector<pair<string, int> > vpair;
    vpair.push_back(make_pair("k1", 123));
    vpair.push_back(make_pair("k1", 124));
    vpair.push_back(make_pair("k1", 125));
    vpair.push_back(make_pair("k2", 125));

    stringstream s;
    s << vpair;

    EXPECT("[<k1,123>,<k1,124>,<k1,125>,<k2,125>]" == s.str());
}

CASE("test_eckit_print_vector_ints") {
    vector<int> vint;
    vint.push_back(123);
    vint.push_back(124);
    vint.push_back(125);
    vint.push_back(126);
    vint.push_back(127);
    vint.push_back(129);
    vint.push_back(131);
    vint.push_back(133);
    vint.push_back(135);
    vint.push_back(135);
    vint.push_back(135);
    vint.push_back(135);
    vint.push_back(1);

    stringstream s;
    s << vint;

    EXPECT("[123-127,129-135-2,3*135,1]" == s.str());
}

CASE("test_eckit_print_vector_doubs") {
    // These should not contract into ranges.
    vector<double> vdoub;
    vdoub.push_back(123.0);
    vdoub.push_back(124.0);
    vdoub.push_back(125.0);
    vdoub.push_back(126.0);
    vdoub.push_back(127.0);
    vdoub.push_back(129.0);
    vdoub.push_back(131.0);
    vdoub.push_back(133.0);
    vdoub.push_back(135.0);
    vdoub.push_back(135.0);
    vdoub.push_back(135.0);
    vdoub.push_back(135.0);
    vdoub.push_back(1.0);

    stringstream s;
    s << vdoub;

    EXPECT("[123,124,125,126,127,129,131,133,135,135,135,135,1]" == s.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
