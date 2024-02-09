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

void ensureClean() {

    auto client = S3Client::makeUnique(cfg);
    auto&& tmp = client->listBuckets();
    std::set<std::string> buckets(tmp.begin(), tmp.end());

    for (const std::string& name : {"failed-bucket", "test-bucket-1", "test-bucket-2"}) {
        if (buckets.find(name) != buckets.end()) {
            client->deleteBucket(name);
        }
    }

}


//----------------------------------------------------------------------------------------------------------------------

CASE("different types") {
    EXPECT_THROWS(S3Client::makeUnique(S3Types::NONE));
    EXPECT_NO_THROW(S3Client::makeUnique(S3Types::AWS));
}

CASE("wrong credentials") {
    ensureClean();
    EXPECT_THROWS(S3Client::makeUnique(S3Types::AWS)->createBucket("failed-bucket"));
}

CASE("create bucket in missing region") {
    ensureClean();

    auto config   = cfg;
    config.setRegion("eu-central-2");

    auto client = S3Client::makeUnique(config);

    EXPECT_THROWS(client->createBucket("test-bucket-1"));
}

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

        EXPECT_EQUAL(buckets[0], "test-bucket-1");
        EXPECT_EQUAL(buckets[1], "test-bucket-2");

        for (auto&& bucket : buckets) {
            client->deleteBucket(bucket);
        }
    }

    {
        const auto buckets = client->listBuckets();
        EXPECT(buckets.empty());
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
    } catch (...) {}
    return ret;
}
