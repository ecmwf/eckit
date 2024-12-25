/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include "eckit/io/Buffer.h"
#include "eckit/io/s3/S3BucketName.h"
#include "eckit/io/s3/S3BucketPath.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/S3ObjectName.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/net/Endpoint.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#define S3_TEST_HOST "minio"
#define S3_TEST_PORT "9000"
#define S3_TEST_ENDPOINT "minio:9000"
#define S3_TEST_REGION "eu-central-1"
#define S3_TEST_BUCKET "eckit-test-s3-bucket"
#define S3_TEST_OBJECT "eckit-test-s3-object"

namespace eckit::test::s3 {

//----------------------------------------------------------------------------------------------------------------------

const net::Endpoint TEST_ENDPOINT {S3_TEST_ENDPOINT};

const S3Config TEST_CONFIG {TEST_ENDPOINT, S3_TEST_REGION};

const S3Credential TEST_CRED {TEST_ENDPOINT, "minio", "minio1234"};

inline bool findString(const std::vector<std::string>& list, const std::string& item) {
    return (std::find(list.begin(), list.end(), item) != list.end());
}

inline void cleanup(const std::vector<std::string>& bucketPaths) {
    auto client = S3Client::makeUnique(TEST_CONFIG);
    for (const auto& path : bucketPaths) {
        if (client->bucketExists(path)) {
            client->emptyBucket(path);
            client->deleteBucket(path);
        }
    }
}

void writePerformance(S3BucketName& bucket, const int count) {
    eckit::Timer timer;

    Buffer buffer(1024 * 1024);
    buffer.zero();

    timer.start();
    for (int i = 0; i < count; i++) {
        const auto objName = S3_TEST_OBJECT + std::to_string(i);
        bucket.makeObject(objName)->put(buffer.data(), buffer.size());
    }
    timer.stop();

    std::cout << "Write performance: " << Bytes(buffer.size()) << " x " << count
              << " objects, rate: " << Bytes(buffer.size() * 1000, timer) << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test::s3
