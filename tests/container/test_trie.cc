/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/container/Trie.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::types;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

// ii) Test removal

CASE("test_trie_insertion") {
    Trie<int> tr;

    EXPECT(tr.empty());

    tr.insert("a-test-string", 1234);

    EXPECT(!tr.empty());

    EXPECT(tr.find("a-test-string") != 0);
    EXPECT(*tr.find("a-test-string") == 1234);

    EXPECT(tr.find("b-test-string") == 0);
    EXPECT(tr.find("a-test-strinh") == 0);
    EXPECT(tr.find("a-test-str") == 0);
    EXPECT(tr.find("a-test-stringy") == 0);

    // Test something that branches off with a lower character value

    tr.insert("a-different-string", 4321);

    EXPECT(tr.find("a-test-string") != 0);
    EXPECT(*tr.find("a-test-string") == 1234);

    EXPECT(tr.find("a-different-string") != 0);
    EXPECT(*tr.find("a-different-string") == 4321);

    // Test something that branches off with a higher charachter value

    tr.insert("a-x-string", 666);

    EXPECT(tr.find("a-test-string") != 0);
    EXPECT(*tr.find("a-test-string") == 1234);

    EXPECT(tr.find("a-different-string") != 0);
    EXPECT(*tr.find("a-different-string") == 4321);

    EXPECT(tr.find("a-x-string") != 0);
    EXPECT(*tr.find("a-x-string") == 666);

    // Test inserting a substring of an existing string

    EXPECT(tr.find("a-x") == 0);

    tr.insert("a-x", 999);

    EXPECT(tr.find("a-x-string") != 0);
    EXPECT(*tr.find("a-x-string") == 666);

    EXPECT(tr.find("a-x") != 0);
    EXPECT(*tr.find("a-x") == 999);

    // Test weird characters

    tr.insert("a-#$@!%^&*", 9876);

    EXPECT(tr.find("a-x-string") != 0);
    EXPECT(*tr.find("a-x-string") == 666);

    EXPECT(tr.find("a-#$@!%^&*") != 0);
    EXPECT(*tr.find("a-#$@!%^&*") == 9876);

    // Test with strings such that an existing entry is a substring.

    tr.insert("a-test-string-extended", 5678);

    EXPECT(tr.find("a-test-string") != 0);
    EXPECT(*tr.find("a-test-string") == 1234);

    EXPECT(tr.find("a-test-string-extended") != 0);
    EXPECT(*tr.find("a-test-string-extended") == 5678);

    // Test insert change first character

    tr.insert("b-test-string", 3141);

    EXPECT(tr.find("a-test-string") != 0);
    EXPECT(*tr.find("a-test-string") == 1234);

    EXPECT(tr.find("b-test-string") != 0);
    EXPECT(*tr.find("b-test-string") == 3141);

    // Test insert change last character

    tr.insert("a-test-strinh", 1413);

    EXPECT(tr.find("a-test-string") != 0);
    EXPECT(*tr.find("a-test-string") == 1234);

    EXPECT(tr.find("a-test-strinh") != 0);
    EXPECT(*tr.find("a-test-strinh") == 1413);
}

CASE("test_trie_value_zero") {
    // Just to check that we aren't using the value_ for making any internal decisions

    Trie<int> tr;

    EXPECT(tr.empty());

    tr.insert("a-str", 0);

    EXPECT(!tr.empty());
    EXPECT(tr.contains("a-str"));
    EXPECT(*tr.find("a-str") == 0);
}

CASE("test_trie_bools") {
    Trie<bool> tr;

    EXPECT(tr.empty());

    tr.insert("a-str", false);
    tr.insert("a-sts", true);

    EXPECT(!tr.empty());

    EXPECT(tr.contains("a-str"));
    EXPECT(*tr.find("a-str") == false);

    EXPECT(tr.contains("a-sts"));
    EXPECT(*tr.find("a-sts") == true);
}

CASE("test_trie_double") {
    Trie<double> tr;

    EXPECT(tr.empty());

    tr.insert("a-str", 12.345);
    tr.insert("a-sts", 5.4321e99);

    EXPECT(!tr.empty());

    EXPECT(tr.contains("a-str"));
    EXPECT(is_approximately_equal(*tr.find("a-str"), 12.345, 1e-10));

    EXPECT(tr.contains("a-sts"));
    EXPECT(is_approximately_equal(*tr.find("a-sts"), 5.4321e99, 1e-10));
}


CASE("test_removal") {
    Trie<int> tr;

    tr.insert("a", 1111);
    tr.insert("a-string", 2222);
    tr.insert("a-strinh", 3333);
    tr.insert("a-string-extended", 4444);
    tr.insert("a-str", 555);

    EXPECT(!tr.empty());

    EXPECT(tr.contains("a"));
    EXPECT(tr.contains("a-string"));
    EXPECT(tr.contains("a-strinh"));
    EXPECT(tr.contains("a-string-extended"));
    EXPECT(tr.contains("a-str"));

    tr.remove("a");

    EXPECT(!tr.empty());
    EXPECT(!tr.contains("a"));
    EXPECT(tr.contains("a-string"));
    EXPECT(tr.contains("a-strinh"));
    EXPECT(tr.contains("a-string-extended"));
    EXPECT(tr.contains("a-str"));

    // Remove a substring before removing the longer one.

    tr.remove("a-string");

    EXPECT(!tr.empty());
    EXPECT(!tr.contains("a"));
    EXPECT(!tr.contains("a-string"));
    EXPECT(tr.contains("a-strinh"));
    EXPECT(tr.contains("a-string-extended"));
    EXPECT(tr.contains("a-str"));

    tr.remove("a-strinh");

    EXPECT(!tr.empty());
    EXPECT(!tr.contains("a"));
    EXPECT(!tr.contains("a-string"));
    EXPECT(!tr.contains("a-strinh"));
    EXPECT(tr.contains("a-string-extended"));
    EXPECT(tr.contains("a-str"));

    tr.remove("a-string-extended");

    EXPECT(!tr.empty());
    EXPECT(!tr.contains("a"));
    EXPECT(!tr.contains("a-string"));
    EXPECT(!tr.contains("a-strinh"));
    EXPECT(!tr.contains("a-string-extended"));
    EXPECT(tr.contains("a-str"));

    // And this time we remove the shorter one later

    tr.remove("a-str");

    EXPECT(!tr.contains("a"));
    EXPECT(!tr.contains("a-string"));
    EXPECT(!tr.contains("a-strinh"));
    EXPECT(!tr.contains("a-string-extended"));
    EXPECT(!tr.contains("a-str"));

    EXPECT(tr.empty());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
