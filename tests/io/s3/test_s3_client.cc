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
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/net/Endpoint.h"
#include "eckit/testing/Test.h"
#include "test_s3_config.h"

#include <string>
#include <vector>

using namespace eckit;
using namespace eckit::testing;

// this test requires a working S3 endpoint and credentials. such as, a local docker container MinIO instance

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

const std::vector<std::string> testBuckets {"test-bucket-1", "test-bucket-2"};

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 client: API") {

    EXPECT(S3Session::instance().addClient(s3::TEST_CONFIG));
    EXPECT(S3Session::instance().addCredential(s3::TEST_CRED));

    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    EXPECT_NO_THROW(S3Session::instance().removeClient(s3::TEST_ENDPOINT));
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

    EXPECT(S3Session::instance().addCredential(s3::TEST_CRED));

    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    EXPECT_NO_THROW(S3Session::instance().removeCredential(s3::TEST_ENDPOINT));
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
    S3Config cfgTmp(s3::TEST_CONFIG);

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
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    // this test requires an S3 endpoint that sets it's region
    // a MinIO instance with empty region will not throw an exception

    auto cfgTmp   = s3::TEST_CONFIG;
    cfgTmp.region = "non-existing-region-random";

    EXPECT_THROWS(S3Client::makeUnique(cfgTmp)->createBucket({"test-bucket-1"}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("create s3 bucket") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    auto client = S3Client::makeUnique(s3::TEST_CONFIG);

    EXPECT_NO_THROW(client->createBucket({"test-bucket-1"}));

    EXPECT_THROWS(client->createBucket({"test-bucket-1"}));

    EXPECT_NO_THROW(client->createBucket({"test-bucket-2"}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("list s3 buckets") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    auto client = S3Client::makeUnique(s3::TEST_CONFIG);
    EXPECT_NO_THROW(client->createBucket({"test-bucket-1"}));
    EXPECT_NO_THROW(client->createBucket({"test-bucket-2"}));

    {
        const auto buckets = client->listBuckets();

        EXPECT(s3::findString(buckets, "test-bucket-1"));
        EXPECT(s3::findString(buckets, {"test-bucket-2"}));

        EXPECT_NO_THROW(client->deleteBucket({"test-bucket-1"}));
        EXPECT_NO_THROW(client->deleteBucket({"test-bucket-2"}));
    }

    {
        const auto buckets = client->listBuckets();
        EXPECT_NOT(s3::findString(buckets, "test-bucket-1"));
        EXPECT_NOT(s3::findString(buckets, {"test-bucket-2"}));
    }

    EXPECT_THROWS(client->deleteBucket({"test-bucket-1"}));
    EXPECT_THROWS(client->deleteBucket({"test-bucket-2"}));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    int ret = -1;

    ret = run_tests(argc, argv);

    test::s3::cleanup(test::testBuckets);

    return ret;
}
