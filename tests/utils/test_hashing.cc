/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <iostream>
#include <memory>

#include "eckit/utils/Hash.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------


std::vector<string> tests = {"",
                             "a",
                             "abc",
                             "message digest",
                             "abcdefghijklmnopqrstuvwxyz",
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                             "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
                             "The quick brown fox jumps over the lazy cog"};

std::map<string, std::vector<string>> results = {
    {"nOnE",
     {
         "",  //""
         "",  //"a"
         "",  //"abc"
         "",  //"message digest"
         "",  //"abcdefghijklmnopqrstuvwxyz"
         "",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "",  //"The quick brown fox jumps over the lazy cog"
         "",  //"The quick brown fox jumps over the lazy cog" x 2

     }},
    {"md4",
     {
         "31d6cfe0d16ae931b73c59d7e0c089c0",  //""
         "bde52cb31de33e46245e05fbdbd6fb24",  //"a"
         "a448017aaf21d8525fc10ae87aa6729d",  //"abc"
         "d9130a8164549fe818874806e1c7014b",  //"message digest"
         "d79e1c308aa5bbcdeea8ed63df412da9",  //"abcdefghijklmnopqrstuvwxyz"
         "043f8582f241db351ce627e153e7f0e4",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "e33b4ddc9c38f2199c3e7b164fcc0536",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "b86e130ce7028da59e672d56ad0113df",  //"The quick brown fox jumps over the lazy cog"
         "16b52abf32d2796d53c6202a877a69a0",  //"The quick brown fox jumps over the lazy cog" x 2

     }},
    {"MD4",
     {
         "31d6cfe0d16ae931b73c59d7e0c089c0",  //""
         "bde52cb31de33e46245e05fbdbd6fb24",  //"a"
         "a448017aaf21d8525fc10ae87aa6729d",  //"abc"
         "d9130a8164549fe818874806e1c7014b",  //"message digest"
         "d79e1c308aa5bbcdeea8ed63df412da9",  //"abcdefghijklmnopqrstuvwxyz"
         "043f8582f241db351ce627e153e7f0e4",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "e33b4ddc9c38f2199c3e7b164fcc0536",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "b86e130ce7028da59e672d56ad0113df",  //"The quick brown fox jumps over the lazy cog"
         "16b52abf32d2796d53c6202a877a69a0",  //"The quick brown fox jumps over the lazy cog" x 2

     }},
    {"MD5",
     {
         "d41d8cd98f00b204e9800998ecf8427e",  //""
         "0cc175b9c0f1b6a831c399e269772661",  //"a"
         "900150983cd24fb0d6963f7d28e17f72",  //"abc"
         "f96b697d7cb7938d525a2f31aaf161d0",  //"message digest"
         "c3fcd3d76192e4007dfb496cca67e13b",  //"abcdefghijklmnopqrstuvwxyz"
         "d174ab98d277d9f5a5611c2c9f419d9f",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "57edf4a22be3c955ac49da2e2107b67a",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "1055d3e698d289f2af8663725127bd4b",  //"The quick brown fox jumps over the lazy cog"
         "f6c40218ba016d723a32a43e3c96a929",  //"The quick brown fox jumps over the lazy cog" x 2
     }},
    {"SHA1",
     {
         "da39a3ee5e6b4b0d3255bfef95601890afd80709",  //""
         "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",  //"a"
         "a9993e364706816aba3e25717850c26c9cd0d89d",  //"abc"
         "c12252ceda8be8994d5fa0290a47231c1d16aae3",  //"message digest"
         "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",  //"abcdefghijklmnopqrstuvwxyz"
         "761c457bf73b14d27e9e9265c46f4b4dda11f940",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "50abf5706a150990a08b2c5ea40fa0e585554732",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3",  //"The quick brown fox jumps over the lazy cog"
         "6f020371ced46a2989a2161a113f34ccd35e0729",  //"The quick brown fox jumps over the lazy cog" x 2
     }},
    {"xxh64",
     {
         "ef46db3751d8e999",  //""
         "d24ec4f1a98c6e5b",  //"a"
         "44bc2cf5ad770999",  //"abc"
         "066ed728fceeb3be",  //"message digest"
         "cfe1f278fa89835c",  //"abcdefghijklmnopqrstuvwxyz"
         "aaa46907d3047814",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "e04a477f19ee145d",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "2dcf47703493b6ca",  //"The quick brown fox jumps over the lazy cog"
         "e32a7da747f1bd6e",  //"The quick brown fox jumps over the lazy cog" x 2
     }},
    {"xxhash",  // Deprecated alias of xxh64
     {
         "ef46db3751d8e999",  //""
         "d24ec4f1a98c6e5b",  //"a"
         "44bc2cf5ad770999",  //"abc"
         "066ed728fceeb3be",  //"message digest"
         "cfe1f278fa89835c",  //"abcdefghijklmnopqrstuvwxyz"
         "aaa46907d3047814",  //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
         "e04a477f19ee145d",  //"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
         "2dcf47703493b6ca",  //"The quick brown fox jumps over the lazy cog"
         "e32a7da747f1bd6e",  //"The quick brown fox jumps over the lazy cog" x 2
     }},
};

void testHash(Hash& hash, std::string hashName) {
    auto hashResults = results.find(hashName);
    if (hashResults == results.end())
        return;

    for (int i = 0; i < tests.size(); i++) {
        std::string testResults = hashResults->second[i];
        //        if (testResults.empty())
        //            break;

        hash.reset();
        hash.add(tests[i].c_str(), tests[i].size());
        EXPECT(testResults == hash.digest());

        // constructor-based initialization
        std::unique_ptr<Hash> hashInitialized{HashFactory::instance().build(hashName, tests[i])};
        EXPECT(testResults == hashInitialized->digest());

        // initialize with dummy data and test reset
        hashInitialized.reset(HashFactory::instance().build(hashName, "dummy data"));
        hashInitialized->reset();
        hashInitialized->add(tests[i].c_str(), tests[i].size());
        EXPECT(testResults == hashInitialized->digest());
    }

    // test incremental add
    {
        std::string testResults = hashResults->second[tests.size()];
        hash.reset();
        hash.add(tests[tests.size() - 1].c_str(), tests[tests.size() - 1].size());
        hash.add(tests[tests.size() - 1].c_str(), tests[tests.size() - 1].size());

        EXPECT(testResults == hash.digest());

        // test two adds (as before) is same as a combination of one compute
        std::string combined;
        combined += tests[tests.size() - 1];
        combined += tests[tests.size() - 1];
        EXPECT(hash.compute(combined.c_str(), combined.size()) == testResults);
    }
}


CASE("Hashing") {
    std::unique_ptr<Hash> hash;

    SECTION("Default Hashing") {
        EXPECT_NO_THROW(hash.reset(HashFactory::instance().build()));
        testHash(*hash, "MD5");
    }

    SECTION("Not Existing Hashing") { EXPECT_THROWS(HashFactory::instance().build("dummy name")); }

    for (auto const& element : results) {
        if (HashFactory::instance().has(element.first)) {
            SECTION(element.first + " Hashing") {
                EXPECT_NO_THROW(hash.reset(HashFactory::instance().build(element.first)));
                testHash(*hash, element.first);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
