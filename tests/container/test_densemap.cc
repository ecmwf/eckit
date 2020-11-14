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

#include "eckit/container/DenseMap.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_container_densemap_0") {

    DenseMap<std::string, int> m;

    EXPECT_NO_THROW(m.insert("two", 2));
    EXPECT_NO_THROW(m.insert("four", 4));
    EXPECT_NO_THROW(m.insert("nine", 9));

    EXPECT_NO_THROW(m.sort());

    EXPECT(m.size() == 3);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_map_string_int") {
    DenseMap<std::string, int> m;

    //

    m.insert("two", 2);

    EXPECT(!m.sorted());

    EXPECT_THROWS(m.size());

    m.sort();

    EXPECT(m.sorted());

    EXPECT(m.get("two") == 2);

    //

    m.insert("four", 4);
    m.insert("nine", 9);

    EXPECT(!m.sorted());

    m.sort();

    EXPECT(m.sorted());

    EXPECT(m.size() == 3);

    EXPECT(m.get("two") == 2);
    EXPECT(m.get("four") == 4);
    EXPECT(m.get("nine") == 9);

    // failed find

    EXPECT(!m.contains("one"));

    // replace an existing value

    m.replace("four", 44);
    m["nine"] = 99;

    EXPECT(m.sorted());  // still sorted

    EXPECT(m.size() == 3);

    EXPECT(m.get("two") == 2);
    EXPECT(m.get("four") == 44);
    EXPECT(m.get("nine") == 99);

    // replace into non-existing

    EXPECT(m.size() == 3);

    m.replace("five", 555);

    // insert into existing

    m.insert("two", 22);

    // find fails if not sorted
    EXPECT(!m.sorted());
    EXPECT_THROWS(m.find("two"));

    m.sort();

    EXPECT(m.get("two") == 22);
    EXPECT(m.get("four") == 44);
    EXPECT(m.get("nine") == 99);
    EXPECT(m.get("five") == 555);

    EXPECT(m.size() == 4);

    // array-like indexing
    EXPECT(m["nine"] == 99);
    EXPECT(m.at(1) == 44);
    EXPECT(m[2] == 99);
    EXPECT_THROWS(m.at(12));

    //
    // std::cout << m << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_map_int_string") {
    DenseMap<int, std::string> m;

    //

    m.insert(2, "two");

    EXPECT(!m.sorted());

    EXPECT_THROWS(m.size());

    m.sort();

    EXPECT(m.sorted());

    EXPECT(m.get(2) == "two");

    //

    m.insert(4, "four");
    m.insert(9, "nine");

    EXPECT(!m.sorted());

    m.sort();

    EXPECT(m.sorted());

    EXPECT(m.size() == 3);

    EXPECT(m.get(2) == "two");
    EXPECT(m.get(4) == "four");
    EXPECT(m.get(9) == "nine");

    // failed find

    EXPECT(m.find(1) == m.end());

    // replace an existing value

    m.replace(4, "FOUR");
    m[9] = "NINE";

    EXPECT(m.sorted());  // still sorted

    EXPECT(m.size() == 3);

    EXPECT(m.get(2) == "two");
    EXPECT(m.get(4) == "FOUR");
    EXPECT(m.get(9) == "NINE");

    // replace into non-existing

    EXPECT(m.size() == 3);

    m.replace(5, "five");

    // insert into existing

    m.insert(2, "TWO");

    // find fails if not sorted
    EXPECT(!m.sorted());
    EXPECT_THROWS(m.find(5));

    m.sort();

    EXPECT(m.get(2) == "TWO");
    EXPECT(m.get(5) == "five");
    EXPECT(m.get(4) == "FOUR");
    EXPECT(m.get(9) == "NINE");
    EXPECT(m.get(9) == "NINE");

    EXPECT(m.size() == 4);

    // iterate over the elements
    std::vector<std::pair<int, std::string> > e = {{2, "TWO"}, {4, "FOUR"}, {5, "five"}, {9, "NINE"}};
    auto map_it                                 = m.begin();
    auto vec_it                                 = e.begin();
    for (; map_it != m.end() && vec_it != e.end(); ++map_it, ++vec_it) {
        EXPECT(map_it->first == vec_it->first);
        EXPECT(map_it->second == vec_it->second);
    }

    // clear the map
    m.clear();
    EXPECT(m.empty());
    EXPECT(m.size() == 0);
    EXPECT(m.sorted());

    // find() should return end() now
    EXPECT(m.find(2) == m.end());

    // std::cout << m << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_map_double_bool") {
    DenseMap<double, bool> m;

    //

    m.insert(2.1, false);

    EXPECT(!m.sorted());

    EXPECT_THROWS(m.size());

    m.sort();

    EXPECT(m.sorted());

    EXPECT(m.get(2.1) == false);

    //

    m.insert(4.0, true);
    m.insert(9.5, false);

    EXPECT(!m.sorted());

    m.sort();

    EXPECT(m.sorted());

    EXPECT(m.size() == 3);

    EXPECT(m.get(2.1) == false);
    EXPECT(m.get(4.0) == true);
    EXPECT(m.get(9.5) == false);

    // failed find

    EXPECT(m.find(1) == m.end());

    // replace an existing value

    m.replace(4.0, false);
    m[9.5] = true;

    EXPECT(m.sorted());  // still sorted

    EXPECT(m.size() == 3);

    EXPECT(m.get(2.1) == false);
    EXPECT(m.get(4.0) == false);
    EXPECT(m.get(9.5) == true);

    // replace into non-existing

    EXPECT(m.size() == 3);

    m.replace(5.3, true);

    // insert into existing

    m.insert(2.1, true);

    // find fails if not sorted
    EXPECT(!m.sorted());
    EXPECT_THROWS(m.find(5.3));

    m.sort();

    EXPECT(m.get(2.1) == true);
    EXPECT(m.get(5.3) == true);
    EXPECT(m.get(4.0) == false);
    EXPECT(m.get(9.5) == true);

    EXPECT(m.size() == 4);

    // iterate over the elements
    std::vector<std::pair<double, bool> > e = {{2.1, true}, {4.0, false}, {5.3, true}, {9.5, true}};
    auto map_it                             = m.begin();
    auto vec_it                             = e.begin();
    for (; map_it != m.end() && vec_it != e.end(); ++map_it, ++vec_it) {
        EXPECT(map_it->first == vec_it->first);
        EXPECT(map_it->second == vec_it->second);
    }

    // clear the map
    m.clear();
    EXPECT(m.empty());
    EXPECT(m.size() == 0);
    EXPECT(m.sorted());

    // find() should return end() now
    EXPECT(m.find(2.1) == m.end());

    // std::cout << m << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
