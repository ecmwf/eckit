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

/// @file   test_s3handle.cc
/// @author Metin Cakircali
/// @date   Jan 2024

#include "eckit/filesystem/URI.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/s3/S3Handle.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/testing/Test.h"

#include <string>

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

S3Config cfg {S3Types::AWS, "test-tag", "eu-central-1", "127.0.0.1", 9000};

static const std::string TEST_BUCKET("test-bucket");

//----------------------------------------------------------------------------------------------------------------------

void ensureClean() {
    auto client = S3Client::makeUnique(cfg);
    auto&& tmp = client->listBuckets();
    std::set<std::string> buckets(tmp.begin(), tmp.end());

    for (const std::string& name : {TEST_BUCKET}) {
        if (buckets.find(name) != buckets.end()) {
            client->deleteBucket(name);
        }
    }
}

void bucketSetup() {
    ensureClean();
    auto client = S3Client::makeUnique(cfg);
    client->createBucket(TEST_BUCKET);
}

CASE("S3Handle") {
    const char buf[] = "abcdefghijklmnopqrstuvwxyz";

    S3Config config("eu-central-1", "127.0.0.1", 9000);

    S3Name name(config, "myS3bucket", "myS3object");

    {
        std::unique_ptr<DataHandle> h{name.dataHandle()};
        h->openForWrite(sizeof(buf));
        AutoClose closer(*h);
        EXPECT(h->write(buf, sizeof(buf)) == sizeof(buf));
    }

    URI uri("s3://127.0.0.1:9000/bucket/object");
    {
        auto h = uri.newReadHandle();
        h->openForRead();
        std::string rbuf;
        h->read(rbuf.data(), sizeof(buf));
        std::cout << rbuf << std::endl;
    }

    {
        std::unique_ptr<DataHandle> dh(uri.newReadHandle());
        MemoryHandle mh;
        dh->saveInto(mh);

        EXPECT(mh.size() == Length(sizeof(buf)));
        EXPECT(::memcmp(mh.data(), buf, sizeof(buf)) == 0);
    }
}

// TODO: Also check that it doesn't work if the bucket doesn't exist, etc.

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    S3Credential cred {"minio", "minio1234", "127.0.0.1"};
    S3Session::instance().addCredentials(cred);

    int ret = -1;
    try {
        eckit::test::bucketSetup();
        ret = run_tests(argc, argv);
        eckit::test::ensureClean();
    } catch (...) {}
    return ret;
}
