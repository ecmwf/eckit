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

#include "eckit/filesystem/URI.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Handle.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/testing/Test.h"

#include <string>

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

S3Config cfg("eu-central-1", "127.0.0.1", 9000);

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

    URI uri("s3://127.0.0.1:9000/test-bucket/test-object");

    {
        S3Name name(uri);

        std::unique_ptr<DataHandle> h {name.dataHandle()};
        h->openForWrite(sizeof(buf));
        AutoClose closer(*h);
        EXPECT(h->write(buf, sizeof(buf)) == sizeof(buf));
    }

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

/// @todo Also check that it doesn't work if the bucket doesn't exist, etc.

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    const S3Credential cred {"minio", "minio1234", "127.0.0.1"};
    S3Session::instance().addCredentials(cred);

    int ret = -1;
    try {
        eckit::test::bucketSetup();
        ret = run_tests(argc, argv);
        eckit::test::ensureClean();
    } catch (...) {}
    return ret;
}
