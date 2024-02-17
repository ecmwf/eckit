/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   test_s3client.cc
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

const S3Config cfg("eu-central-1", "127.0.0.1", 9000);

//----------------------------------------------------------------------------------------------------------------------

bool findString(const std::vector<std::string>& list, const std::string& item) {
    return (std::find(list.begin(), list.end(), item) != list.end());
}

void ensureClean() {

    auto client = S3Client::makeUnique(cfg);
    auto&& tmp = client->listBuckets();
    std::set<std::string> buckets(tmp.begin(), tmp.end());

    for (const std::string& name : {"test-bucket-1", "test-bucket-2"}) {
        if (buckets.find(name) != buckets.end()) {
            client->emptyBucket(name);
            client->deleteBucket(name);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("different types") {
    S3Config cfgTmp(cfg);

    cfgTmp.type = S3Types::NONE;
    EXPECT_THROWS(S3Client::makeUnique(cfgTmp));

    cfgTmp.type = S3Types::AWS;
    EXPECT_NO_THROW(S3Client::makeUnique(cfgTmp));

    cfgTmp.type = S3Types::REST;
    EXPECT_THROWS(S3Client::makeUnique(cfgTmp));
}

// CASE("wrong credentials") {
//     ensureClean();
//
//     S3Config cfgTmp(cfg);
//     cfgTmp.region = "no-region-random";
//
//     EXPECT_THROWS(S3Client::makeUnique(cfgTmp)->createBucket("failed-bucket"));
// }

// CASE("create bucket in non-existing region") {
//     ensureClean();

//     S3Config cfgTmp(cfg);
//     cfgTmp.region = "non-existing-region-random";

//     EXPECT_THROWS(S3Client::makeUnique(cfgTmp)->createBucket("test-bucket-1"));
// }

CASE("create bucket") {
    ensureClean();

    auto client = S3Client::makeUnique(cfg);

    EXPECT_NO_THROW(client->createBucket("test-bucket-1"));

    EXPECT_THROWS(client->createBucket("test-bucket-1"));

    EXPECT_NO_THROW(client->createBucket("test-bucket-2"));
}

CASE("list buckets") {
    ensureClean();

    auto client = S3Client::makeUnique(cfg);
    EXPECT_NO_THROW(client->createBucket("test-bucket-1"));
    EXPECT_NO_THROW(client->createBucket("test-bucket-2"));

    {
        const auto buckets = client->listBuckets();

        EXPECT(findString(buckets, "test-bucket-1"));
        EXPECT(findString(buckets, "test-bucket-2"));

        EXPECT_NO_THROW(client->deleteBucket("test-bucket-1"));
        EXPECT_NO_THROW(client->deleteBucket("test-bucket-2"));
    }

    {
        const auto buckets = client->listBuckets();
        EXPECT_NOT(findString(buckets, "test-bucket-1"));
        EXPECT_NOT(findString(buckets, "test-bucket-2"));
    }

    EXPECT_THROWS(client->deleteBucket("test-bucket-1"));
    EXPECT_THROWS(client->deleteBucket("test-bucket-2"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    const S3Credential cred {"minio", "minio1234", "127.0.0.1"};
    S3Session::instance().addCredentials(cred);

    auto ret = run_tests(argc, argv);
    try {
        eckit::test::ensureClean();
    } catch (...) { }
    return ret;
}
