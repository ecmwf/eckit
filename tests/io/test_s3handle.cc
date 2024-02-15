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
#include "eckit/io/Buffer.h"
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

CASE("bucket exists") {
    ensureClean();

    S3Client::makeUnique(cfg)->createBucket(TEST_BUCKET);

    const void* buffer = TEST_DATA.data();
    const long  length = TEST_DATA.size();

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

        const auto rlen = h->openForRead();
        EXPECT(rlen == Length(length));

        std::string rbuf;
        rbuf.resize(length);

        const auto len = h->read(rbuf.data(), length);
        /// @todo this is odd with respect to saveInto issue below
        // EXPECT(len == 0);  // POSIX
        EXPECT(len == length);

        EXPECT(rbuf == TEST_DATA);
    }

    {
        std::unique_ptr<DataHandle> dh(uri.newReadHandle());

        MemoryHandle mh(length);
        /// @todo this creates request loop issues because default internal buffer size is 64MiB
        dh->saveInto(mh);

        EXPECT(mh.size() == Length(length));
        EXPECT(::memcmp(mh.data(), buffer, length) == 0);
    }
}

CASE("bucket does not exist") {
    URI uri("s3://127.0.0.1:9000/" + TEST_BUCKET + "/" + TEST_OBJECT);

    ensureClean();

    std::unique_ptr<DataHandle> h(uri.newReadHandle());
    EXPECT_THROWS(h->openForRead());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    const S3Credential cred {"minio", "minio1234", "127.0.0.1"};
    S3Session::instance().addCredentials(cred);

    int ret = -1;
    try {
        eckit::test::ensureClean();
        ret = run_tests(argc, argv);
    } catch (...) {}
    return ret;
}
