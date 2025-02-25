/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   test_s3handle.cc
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/s3/S3BucketName.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3ObjectName.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/net/Endpoint.h"
#include "eckit/testing/Test.h"
#include "test_s3_config.h"

#include <bits/basic_string.h>

#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_literals;

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

constexpr std::string_view TEST_DATA {"abcdefghijklmnopqrstuvwxyz"};

const std::string TEST_BUCKET {"eckit-s3handle-test-bucket"};
const std::string TEST_OBJECT {"eckit-s3handle-test-object"};

const std::vector<std::string> testBuckets {TEST_BUCKET};

//----------------------------------------------------------------------------------------------------------------------

CASE("initialize s3 session") {

    EXPECT(S3Session::instance().addCredential(s3::TEST_CRED));
    EXPECT(S3Session::instance().addClient(s3::TEST_CONFIG));

    EXPECT_NO_THROW(s3::cleanup(testBuckets));
}

CASE("invalid s3 bucket") {
    EXPECT_THROWS(S3BucketName(URI {"http://127.0.0.1:9000/" + TEST_BUCKET}));
    EXPECT_THROWS(S3BucketName(URI {"s3://127.0.0.1" + TEST_BUCKET}));
    EXPECT_THROWS(S3BucketName(URI {"s3://127.0.0.1/" + TEST_BUCKET}));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3BucketName: no bucket") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    S3BucketName bucket(s3::TEST_ENDPOINT, TEST_BUCKET);

    EXPECT_NOT(bucket.exists());

    // LIST
    EXPECT_THROWS(bucket.listObjects());

    // CREATE OBJECT
    EXPECT_THROWS(bucket.makeObject(TEST_OBJECT)->put(TEST_DATA.data(), TEST_DATA.size()));

    // DESTROY BUCKET
    EXPECT_THROWS(bucket.destroy());
    EXPECT_NO_THROW(bucket.ensureDestroyed());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3BucketName: create bucket") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    S3BucketName bucket(s3::TEST_ENDPOINT, TEST_BUCKET);

    EXPECT_NOT(bucket.exists());

    // CREATE BUCKET
    EXPECT_NO_THROW(bucket.create());
    EXPECT_THROWS(bucket.create());
    EXPECT_NO_THROW(bucket.ensureCreated());

    // DESTROY BUCKET
    EXPECT_NO_THROW(bucket.destroy());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3BucketName: empty bucket") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    S3BucketName bucket(s3::TEST_ENDPOINT, TEST_BUCKET);

    // CREATE BUCKET
    EXPECT_NO_THROW(bucket.ensureCreated());
    EXPECT(bucket.exists());

    // LIST
    EXPECT_EQUAL(bucket.listObjects().size(), 0);

    // DESTROY BUCKET
    EXPECT_NO_THROW(bucket.destroy());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3BucketName: bucket with object") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    S3BucketName bucket(s3::TEST_ENDPOINT, TEST_BUCKET);

    // CREATE BUCKET
    EXPECT_NO_THROW(bucket.ensureCreated());

    // CREATE OBJECT
    EXPECT_NO_THROW(bucket.makeObject(TEST_OBJECT)->put(TEST_DATA.data(), TEST_DATA.size()));

    // LIST
    EXPECT_EQUAL(bucket.listObjects().size(), 1);

    // DESTROY BUCKET
    EXPECT_THROWS(bucket.destroy());
    EXPECT_NO_THROW(bucket.ensureDestroyed());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3Handle: basic operations") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    S3Client::makeUnique(s3::TEST_CONFIG)->createBucket(TEST_BUCKET);

    const void* buffer = TEST_DATA.data();
    const long  length = TEST_DATA.size();

    const URI uri("s3://" + std::string(s3::TEST_ENDPOINT) + "/" + TEST_BUCKET + "/" + TEST_OBJECT);

    {
        S3ObjectName object(uri);

        std::unique_ptr<DataHandle> handle(object.dataHandle());
        EXPECT_NO_THROW(handle->openForWrite(length));
        EXPECT_EQUAL(handle->write(buffer, length), length);
        EXPECT_NO_THROW(handle->close());
    }

    {
        std::unique_ptr<DataHandle> handle(uri.newReadHandle());
        EXPECT_NO_THROW(handle->openForRead());

        std::string rbuf;
        rbuf.resize(length);

        EXPECT_NO_THROW(handle->read(rbuf.data(), length));

        EXPECT_EQUAL(rbuf, TEST_DATA);

        EXPECT_NO_THROW(handle->close());
    }

    {
        std::unique_ptr<DataHandle> handle(uri.newReadHandle());

        MemoryHandle memHandle(length);
        handle->saveInto(memHandle);

        EXPECT_EQUAL(memHandle.size(), Length(length));
        EXPECT_EQUAL(::memcmp(memHandle.data(), buffer, length), 0);

        EXPECT_NO_THROW(handle->close());
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3Handle: openForRead") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    const URI uri("s3://" + std::string(s3::TEST_ENDPOINT) + "/" + TEST_BUCKET + "/" + TEST_OBJECT);

    EXPECT_NO_THROW(S3BucketName(uri).ensureCreated());

    std::unique_ptr<DataHandle> handle(uri.newReadHandle());

    // NO OBJECT
    EXPECT_THROWS(handle->openForRead());

    // CREATE OBJECT
    EXPECT_NO_THROW(S3ObjectName(uri).put(nullptr, 0));

    // DOUBLE OPEN
    EXPECT_THROWS(handle->openForRead());

    // RE-OPEN
    EXPECT_NO_THROW(handle->close());
    EXPECT_NO_THROW(handle->openForRead());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3Handle: openForWrite") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    const URI uri("s3://" + std::string(s3::TEST_ENDPOINT) + "/" + TEST_BUCKET + "/" + TEST_OBJECT);

    {  // NO BUCKET
        std::unique_ptr<DataHandle> handle(uri.newWriteHandle());
        EXPECT_THROWS(handle->openForWrite(0));
        EXPECT_NO_THROW(handle->close());
    }

    // CREATE BUCKET
    EXPECT_NO_THROW(S3BucketName(uri).ensureCreated());

    {  // BUCKET EXISTS
        std::unique_ptr<DataHandle> handle(uri.newWriteHandle());
        EXPECT_NO_THROW(handle->openForWrite(0));
        EXPECT_NO_THROW(handle->close());
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("S3Handle: read") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    const URI uri("s3://" + std::string(s3::TEST_ENDPOINT) + "/" + TEST_BUCKET + "/" + TEST_OBJECT);

    EXPECT_NO_THROW(S3BucketName(uri).ensureCreated());

    // CREATE OBJECT
    EXPECT_NO_THROW(S3ObjectName(uri).put(TEST_DATA.data(), TEST_DATA.size()));

    std::unique_ptr<DataHandle> handle(uri.newReadHandle());

    // OPEN
    EXPECT_NO_THROW(handle->openForRead());

    /// @todo range based read
    // {
    // const auto length = TEST_DATA.size();
    //
    // std::string rbuf;
    // rbuf.resize(length);
    // auto len = handle->read(rbuf.data(), 10);
    // std::cout << "========" << rbuf << std::endl;
    // len = handle->read(rbuf.data(), length - 10);
    // std::cout << "========" << rbuf << std::endl;
    // EXPECT_EQUAL(rbuf, TEST_DATA);
    // }

    // CLOSE
    EXPECT_NO_THROW(handle->close());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("s3 performance: write 1 10 100 objects") {
    EXPECT_NO_THROW(s3::cleanup(testBuckets));

    const URI uri("s3://" + std::string(s3::TEST_ENDPOINT) + "/" + TEST_BUCKET);

    S3BucketName bucket(uri);

    EXPECT_NO_THROW(bucket.ensureCreated());

    s3::writePerformance(bucket, 1);

    s3::writePerformance(bucket, 10);

    s3::writePerformance(bucket, 100);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    int ret = -1;

    ret = run_tests(argc, argv);

    test::s3::cleanup(test::testBuckets);

    return ret;
}

//----------------------------------------------------------------------------------------------------------------------
