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

#include "eckit/io/s3/S3BucketPath.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/net/Endpoint.h"
#include "eckit/testing/Test.h"
#include "test_s3_config.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace eckit;
using namespace eckit::testing;

// this test requires a working S3 endpoint and credentials. such as, a local docker container MinIO instance

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

const net::Endpoint TEST_ENDPOINT {S3_TEST_ENDPOINT};

const S3Config TEST_CONFIG {TEST_ENDPOINT, S3_TEST_REGION};

const S3Credential TEST_CRED {TEST_ENDPOINT, "minio", "minio1234"};

bool findString(const std::vector<std::string>& list, const std::string& item) {
    return (std::find(list.begin(), list.end(), item) != list.end());
}

void cleanup() {
    auto client = S3Client::makeUnique(TEST_CONFIG);
    for (const auto& name : {S3BucketPath {"test-bucket-1"}, S3BucketPath {{"test-bucket-2"}}}) {
        if (client->bucketExists(name)) {
            client->emptyBucket(name);
            client->deleteBucket(name);
        }
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 client: API") {

    EXPECT(S3Session::instance().addClient(TEST_CONFIG));
    EXPECT(S3Session::instance().addCredential(TEST_CRED));

    EXPECT_NO_THROW(cleanup());

    EXPECT_NO_THROW(S3Session::instance().removeClient(TEST_ENDPOINT));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 client: read from file") {

    auto& session = S3Session::instance();

    session.clear();

    EXPECT_THROWS_AS(session.getClient({"127.0.0.1", 9000}), S3EntityNotFound);
    EXPECT_THROWS_AS(session.getClient({"minio", 9000}), S3EntityNotFound);
    EXPECT_THROWS_AS(session.getClient({"localhost", 9000}), S3EntityNotFound);

    session.loadClients("./S3Config.yaml");

    EXPECT_NO_THROW(session.getClient({"127.0.0.1", 9000}));
    EXPECT_NO_THROW(session.getClient({"minio", 9000}));
    EXPECT_NO_THROW(session.getClient({"localhost", 9000}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 credentials: API") {

    EXPECT(S3Session::instance().addCredential(TEST_CRED));

    EXPECT_NO_THROW(cleanup());

    EXPECT_NO_THROW(S3Session::instance().removeCredential(TEST_ENDPOINT));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 credentials: read from file") {

    auto& session = S3Session::instance();

    session.clear();

    EXPECT_THROWS_AS(session.getCredential({"127.0.0.1", 9000}), S3EntityNotFound);
    EXPECT_THROWS_AS(session.getCredential({"minio", 9000}), S3EntityNotFound);
    EXPECT_THROWS_AS(session.getCredential({"localhost", 9000}), S3EntityNotFound);

    session.loadCredentials("./S3Credentials.yaml");

    EXPECT_NO_THROW(session.getCredential({"127.0.0.1", 9000}));
    EXPECT_NO_THROW(session.getCredential({"minio", 9000}));
    EXPECT_NO_THROW(session.getCredential({"localhost", 9000}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 backends") {
    S3Config cfgTmp(TEST_CONFIG);

    cfgTmp.backend = S3Backend::AWS;
    EXPECT_NO_THROW(S3Client::makeUnique(cfgTmp));

    cfgTmp.backend = S3Backend::REST;
    EXPECT_THROWS(S3Client::makeUnique(cfgTmp));

    cfgTmp.backend = S3Backend::MINIO;
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

//----------------------------------------------------------------------------------------------------------------------

CASE("create s3 bucket in non-existing region") {
    EXPECT_NO_THROW(cleanup());

    // this test requires an S3 endpoint that sets it's region
    // a MinIO instance with empty region will not throw an exception

    auto cfgTmp   = TEST_CONFIG;
    cfgTmp.region = "non-existing-region-random";

    EXPECT_THROWS(S3Client::makeUnique(cfgTmp)->createBucket({"test-bucket-1"}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("create s3 bucket") {
    EXPECT_NO_THROW(cleanup());

    auto client = S3Client::makeUnique(TEST_CONFIG);

    EXPECT_NO_THROW(client->createBucket({"test-bucket-1"}));

    EXPECT_THROWS(client->createBucket({"test-bucket-1"}));

    EXPECT_NO_THROW(client->createBucket({"test-bucket-2"}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("list s3 buckets") {
    EXPECT_NO_THROW(cleanup());

    auto client = S3Client::makeUnique(TEST_CONFIG);
    EXPECT_NO_THROW(client->createBucket({"test-bucket-1"}));
    EXPECT_NO_THROW(client->createBucket({"test-bucket-2"}));

    {
        const auto buckets = client->listBuckets();

        EXPECT(findString(buckets, "test-bucket-1"));
        EXPECT(findString(buckets, {"test-bucket-2"}));

        EXPECT_NO_THROW(client->deleteBucket({"test-bucket-1"}));
        EXPECT_NO_THROW(client->deleteBucket({"test-bucket-2"}));
    }

    {
        const auto buckets = client->listBuckets();
        EXPECT_NOT(findString(buckets, "test-bucket-1"));
        EXPECT_NOT(findString(buckets, {"test-bucket-2"}));
    }

    EXPECT_THROWS(client->deleteBucket({"test-bucket-1"}));
    EXPECT_THROWS(client->deleteBucket({"test-bucket-2"}));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    int ret = -1;

    ret = run_tests(argc, argv);

    test::cleanup();

    return ret;
}
