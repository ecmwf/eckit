/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file   test_s3client.cc
/// @author Metin Cakircali
/// @date   Jan 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/testing/Test.h"

#include <cstring>

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

S3Config cfg {S3Types::AWS, "eu-central-1", "127.0.0.1", 9000};

//----------------------------------------------------------------------------------------------------------------------

CASE("unkown S3 type") {
    EXPECT_THROWS(S3Client::makeUnique({S3Types::NONE}));
}

CASE("create bucket") {
    auto client = S3Client::makeUnique(cfg);

    EXPECT(client->createBucket("test-bucket-1"));

    EXPECT_NOT(client->createBucket("test-bucket-1"));

    EXPECT(client->createBucket("test-bucket-2"));
}

CASE("list buckets") {
    auto client = S3Client::makeUnique(cfg);

    const auto buckets = client->listBuckets();

    EXPECT_EQUAL(buckets[0], "test-bucket-1");
    EXPECT_EQUAL(buckets[1], "test-bucket-2");

    for (auto&& bucket : buckets) { EXPECT(client->deleteBucket(bucket)); }

    EXPECT_NOT(client->deleteBucket("test-bucket-1"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
