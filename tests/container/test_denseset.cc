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

#include "eckit/container/DenseSet.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_container_denseset_0") {

    DenseSet<std::string> s;

    EXPECT_NO_THROW(s.insert("two"));
    EXPECT_NO_THROW(s.insert("four"));
    EXPECT_NO_THROW(s.insert("nine"));

    EXPECT(s.size() == 3);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_set_string") {
    DenseSet<std::string> s;

    //

    s.insert("two");

    EXPECT(!s.sorted());

    EXPECT(s.size() == 1);

    s.sort();

    EXPECT(s.sorted());

    EXPECT(s.contains("two"));

    //

    s.insert("four");
    s.insert("nine");

    EXPECT(!s.sorted());

    s.sort();

    EXPECT(s.sorted());

    EXPECT(s.size() == 3);

    EXPECT(s.find("two") != s.end());
    EXPECT(s.find("four") != s.end());
    EXPECT(s.find("nine") != s.end());

    // failed find

    EXPECT(!s.contains("one"));
    EXPECT(s.find("one") == s.end());

    // iterate over the elements
    std::vector<std::string> e = {"four", "nine", "two"};
    auto set_it = s.begin();
    auto vec_it = e.begin();
    for(; set_it != s.end() && vec_it != e.end(); ++set_it, ++vec_it) {
        EXPECT(*set_it == *vec_it);
    }

    // array-like indexing
    EXPECT(s[1] == "nine");
    EXPECT(s.at(2) == "two");
    EXPECT_THROWS(s.at(12));

    // clear the map
    s.clear();
    EXPECT(s.empty());
    EXPECT(s.size() == 0);
    EXPECT(s.sorted());

    //
    // std::cout << s << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_set_int") {
    DenseSet<int> s;

    //

    s.insert(2);

    EXPECT(!s.sorted());

    EXPECT(s.size() == 1);

    s.sort();

    EXPECT(s.sorted());

    EXPECT(s.contains(2));

    //

    s.insert(4);
    s.insert(9);

    EXPECT(!s.sorted());

    s.sort();

    EXPECT(s.sorted());

    EXPECT(s.size() == 3);

    EXPECT(s.find(2) != s.end());
    EXPECT(s.find(4) != s.end());
    EXPECT(s.find(9) != s.end());

    // failed find

    EXPECT(!s.contains(1));
    EXPECT(s.find(1) == s.end());

    // iterate over the elements
    std::vector<int> e = {2, 4, 9};
    auto set_it = s.begin();
    auto vec_it = e.begin();
    for(; set_it != s.end() && vec_it != e.end(); ++set_it, ++vec_it) {
        EXPECT(*set_it == *vec_it);
    }

    // array-like indexing
    EXPECT(s[1] == 4);
    EXPECT(s.at(2) == 9);

    // clear the map
    s.clear();
    EXPECT(s.empty());
    EXPECT(s.size() == 0);
    EXPECT(s.sorted());

    // std::cout << s << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
