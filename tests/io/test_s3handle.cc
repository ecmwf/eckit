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

constexpr std::string_view TEST_DATA = "abcdefghijklmnopqrstuvwxyz";

static const std::string TEST_BUCKET("eckit-s3handle-test-bucket");
static const std::string TEST_OBJECT("eckit-s3handle-test-object");

//----------------------------------------------------------------------------------------------------------------------

void ensureClean() {
    auto client = S3Client::makeUnique(cfg);
    auto&& tmp = client->listBuckets();
    std::set<std::string> buckets(tmp.begin(), tmp.end());

    for (const std::string& name : {TEST_BUCKET}) {
        if (buckets.find(name) != buckets.end()) {
            client->emptyBucket(name);
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
    const void* buffer = TEST_DATA.data();
    const auto  length = TEST_DATA.size();

    std::cout << "write buffer: " << TEST_DATA << std::endl;

    URI uri("s3://127.0.0.1:9000/" + TEST_BUCKET + "/" + TEST_OBJECT);

    {
        S3Name name(uri);

        std::unique_ptr<DataHandle> h(name.dataHandle());
        h->openForWrite(length);
        AutoClose closer(*h);
        EXPECT(h->write(buffer, length) == length);
    }

    {
        std::unique_ptr<DataHandle> h(uri.newReadHandle());
        h->openForRead();
        std::string rbuf;
        std::cout << "read buffer: " << rbuf << std::endl;
        h->read(rbuf.data(), length);
        std::cout << "read buffer: " << rbuf << std::endl;
    }

    {
        std::unique_ptr<DataHandle> dh(uri.newReadHandle());
        MemoryHandle mh;
        dh->saveInto(mh);

        EXPECT(mh.size() == Length(length));
        EXPECT(::memcmp(mh.data(), buffer, length) == 0);
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
