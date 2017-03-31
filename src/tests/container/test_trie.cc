/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE eckit_test_trie

#include "ecbuild/boost_test_framework.h"

#include "eckit/container/Trie.h"
#include "eckit/testing/Setup.h"
#include "eckit/log/Log.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

//-----------------------------------------------------------------------------

// ii) Test removal

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_container_trie )

BOOST_AUTO_TEST_CASE( test_trie_insertion )
{
    Trie<int> tr;

    BOOST_CHECK(tr.empty());

    tr.insert("a-test-string", 1234);

    BOOST_CHECK(!tr.empty());

    BOOST_CHECK(tr.find("a-test-string") != 0);
    BOOST_CHECK(*tr.find("a-test-string") == 1234);

    BOOST_CHECK(tr.find("b-test-string") == 0);
    BOOST_CHECK(tr.find("a-test-strinh") == 0);
    BOOST_CHECK(tr.find("a-test-str") == 0);
    BOOST_CHECK(tr.find("a-test-stringy") == 0);

    // Test something that branches off with a lower character value

    tr.insert("a-different-string", 4321);

    BOOST_CHECK(tr.find("a-test-string") != 0);
    BOOST_CHECK(*tr.find("a-test-string") == 1234);

    BOOST_CHECK(tr.find("a-different-string") != 0);
    BOOST_CHECK(*tr.find("a-different-string") == 4321);

    // Test something that branches off with a higher charachter value

    tr.insert("a-x-string", 666);

    BOOST_CHECK(tr.find("a-test-string") != 0);
    BOOST_CHECK(*tr.find("a-test-string") == 1234);

    BOOST_CHECK(tr.find("a-different-string") != 0);
    BOOST_CHECK(*tr.find("a-different-string") == 4321);

    BOOST_CHECK(tr.find("a-x-string") != 0);
    BOOST_CHECK(*tr.find("a-x-string") == 666);

    // Test inserting a substring of an existing string

    BOOST_CHECK(tr.find("a-x") == 0);

    tr.insert("a-x", 999);

    BOOST_CHECK(tr.find("a-x-string") != 0);
    BOOST_CHECK(*tr.find("a-x-string") == 666);

    BOOST_CHECK(tr.find("a-x") != 0);
    BOOST_CHECK(*tr.find("a-x") == 999);

    // Test weird characters

    tr.insert("a-#$@!%^&*", 9876);

    BOOST_CHECK(tr.find("a-x-string") != 0);
    BOOST_CHECK(*tr.find("a-x-string") == 666);

    BOOST_CHECK(tr.find("a-#$@!%^&*") != 0);
    BOOST_CHECK(*tr.find("a-#$@!%^&*") == 9876);

    // Test with strings such that an existing entry is a substring.

    tr.insert("a-test-string-extended", 5678);

    BOOST_CHECK(tr.find("a-test-string") != 0);
    BOOST_CHECK(*tr.find("a-test-string") == 1234);

    BOOST_CHECK(tr.find("a-test-string-extended") != 0);
    BOOST_CHECK(*tr.find("a-test-string-extended") == 5678);

    // Test insert change first character

    tr.insert("b-test-string", 3141);

    BOOST_CHECK(tr.find("a-test-string") != 0);
    BOOST_CHECK(*tr.find("a-test-string") == 1234);

    BOOST_CHECK(tr.find("b-test-string") != 0);
    BOOST_CHECK(*tr.find("b-test-string") == 3141);

    // Test insert change last character

    tr.insert("a-test-strinh", 1413);

    BOOST_CHECK(tr.find("a-test-string") != 0);
    BOOST_CHECK(*tr.find("a-test-string") == 1234);

    BOOST_CHECK(tr.find("a-test-strinh") != 0);
    BOOST_CHECK(*tr.find("a-test-strinh") == 1413);

}

BOOST_AUTO_TEST_CASE( test_trie_value_zero )
{
    // Just to check that we aren't using the value_ for making any internal decisions

    Trie<int> tr;

    BOOST_CHECK(tr.empty());

    tr.insert("a-str", 0);

    BOOST_CHECK(!tr.empty());
    BOOST_CHECK(tr.contains("a-str"));
    BOOST_CHECK(*tr.find("a-str") == 0);

}

BOOST_AUTO_TEST_CASE( test_trie_bools )
{
    Trie<bool> tr;

    BOOST_CHECK(tr.empty());

    tr.insert("a-str", false);
    tr.insert("a-sts", true);

    BOOST_CHECK(!tr.empty());

    BOOST_CHECK(tr.contains("a-str"));
    BOOST_CHECK(*tr.find("a-str") == false);

    BOOST_CHECK(tr.contains("a-sts"));
    BOOST_CHECK(*tr.find("a-sts") == true);
}

BOOST_AUTO_TEST_CASE( test_trie_double )
{
    Trie<double> tr;

    BOOST_CHECK(tr.empty());

    tr.insert("a-str", 12.345);
    tr.insert("a-sts", 5.4321e99);

    BOOST_CHECK(!tr.empty());

    BOOST_CHECK(tr.contains("a-str"));
    BOOST_CHECK_CLOSE(*tr.find("a-str") , 12.345, 1e-10);

    BOOST_CHECK(tr.contains("a-sts"));
    BOOST_CHECK_CLOSE(*tr.find("a-sts"), 5.4321e99, 1e-10);
}


BOOST_AUTO_TEST_CASE( test_removal )
{
    Trie<int> tr;

    tr.insert("a", 1111);
    tr.insert("a-string", 2222);
    tr.insert("a-strinh", 3333);
    tr.insert("a-string-extended", 4444);
    tr.insert("a-str", 555);

    BOOST_CHECK(!tr.empty());

    BOOST_CHECK(tr.contains("a"));
    BOOST_CHECK(tr.contains("a-string"));
    BOOST_CHECK(tr.contains("a-strinh"));
    BOOST_CHECK(tr.contains("a-string-extended"));
    BOOST_CHECK(tr.contains("a-str"));

    tr.remove("a");

    BOOST_CHECK(!tr.empty());
    BOOST_CHECK(!tr.contains("a"));
    BOOST_CHECK(tr.contains("a-string"));
    BOOST_CHECK(tr.contains("a-strinh"));
    BOOST_CHECK(tr.contains("a-string-extended"));
    BOOST_CHECK(tr.contains("a-str"));

    // Remove a substring before removing the longer one.

    tr.remove("a-string");

    BOOST_CHECK(!tr.empty());
    BOOST_CHECK(!tr.contains("a"));
    BOOST_CHECK(!tr.contains("a-string"));
    BOOST_CHECK(tr.contains("a-strinh"));
    BOOST_CHECK(tr.contains("a-string-extended"));
    BOOST_CHECK(tr.contains("a-str"));

    tr.remove("a-strinh");

    BOOST_CHECK(!tr.empty());
    BOOST_CHECK(!tr.contains("a"));
    BOOST_CHECK(!tr.contains("a-string"));
    BOOST_CHECK(!tr.contains("a-strinh"));
    BOOST_CHECK(tr.contains("a-string-extended"));
    BOOST_CHECK(tr.contains("a-str"));

    tr.remove("a-string-extended");

    BOOST_CHECK(!tr.empty());
    BOOST_CHECK(!tr.contains("a"));
    BOOST_CHECK(!tr.contains("a-string"));
    BOOST_CHECK(!tr.contains("a-strinh"));
    BOOST_CHECK(!tr.contains("a-string-extended"));
    BOOST_CHECK(tr.contains("a-str"));

    // And this time we remove the shorter one later

    tr.remove("a-str");

    BOOST_CHECK(!tr.contains("a"));
    BOOST_CHECK(!tr.contains("a-string"));
    BOOST_CHECK(!tr.contains("a-strinh"));
    BOOST_CHECK(!tr.contains("a-string-extended"));
    BOOST_CHECK(!tr.contains("a-str"));

    BOOST_CHECK(tr.empty());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
