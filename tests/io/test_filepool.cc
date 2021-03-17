/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cmath>
#include <cstring>
#include <thread>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/FilePool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static const size_t BUF_SIZE = 1024;

static const char* files[] = {"foo.data", "bar.data", "baz.data", "marco.data", "polo.data"};

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_io_filepool_threads") {

    FilePool pool(1);

    size_t nThreads = Resource<size_t>("$ECKIT_TEST_THREADS", 16);

    nThreads = std::min(size_t(255), nThreads);

    std::vector<std::thread> threads;

    for (size_t i = 0; i < nThreads; ++i) {
        threads.emplace_back(std::thread([&pool, i]() {
            std::vector<unsigned char> buffer(BUF_SIZE, i + 1);
            DataHandle* foo = pool.checkout("foo.data");
            foo->write(&buffer[0], buffer.size());
            pool.checkin(foo);
        }));
    }

    for (auto& t : threads) {
        t.join();
    }

    // verfy the data

    DataHandle* foo = pool.checkout("foo.data");
    EXPECT(foo->openForRead() >= Length(nThreads * BUF_SIZE));

    // Check we have nThreads blocks of BUF_SIZE with Bytes 1 to nThreads
    std::vector<bool> found(nThreads);
    for (size_t i = 0; i < nThreads; ++i) {
        std::vector<unsigned char> buffer(BUF_SIZE);
        foo->read(&buffer[0], BUF_SIZE);
        const unsigned char c = buffer[0];
        EXPECT(c > 0);
        EXPECT(!found[c - 1]);
        std::vector<unsigned char> expect(BUF_SIZE, c);
        EXPECT(buffer == expect);
        found[c - 1] = true;
    }
    for (size_t i = 0; i < nThreads; ++i) {
        EXPECT(found[i]);
    }

    pool.checkin(foo);
    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 1);
}

CASE("test_eckit_io_filepool_0") {
    Buffer buffer(BUF_SIZE);
    ::memset(buffer, 0, buffer.size());

    FilePool pool(1);

    // pool sized 1

    EXPECT(pool.capacity() == 1);
    EXPECT(pool.size() == 0);

    pool.capacity(3);

    // pool is larger

    EXPECT(pool.capacity() == 3);
    EXPECT(pool.size() == 0);

    DataHandle* foo = pool.checkout("foo.data");
    foo->write(buffer, buffer.size());

    // foo is in use

    EXPECT(pool.usage() == 1);
    EXPECT(pool.size() == 0);

    DataHandle* bar = pool.checkout("bar.data");
    bar->write(buffer, buffer.size());

    // foo + bar are in use

    EXPECT(pool.usage() == 2);
    EXPECT(pool.size() == 0);

    Log::info() << pool << std::endl;

    pool.checkin(foo);
    pool.checkin(bar);

    Log::info() << pool << std::endl;

    // foo + bar are in pool, none in use

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 2);

    DataHandle* foo2 = pool.checkout("foo.data");
    foo2->write(buffer, buffer.size());

    // foo in use again, bar in pool

    EXPECT(pool.usage() == 1);
    EXPECT(pool.size() == 1);

    pool.checkin(foo);

    // foo + bar are again in pool, none in use

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 2);

    DataHandle* baz = pool.checkout("baz.data");
    baz->write(buffer, buffer.size());

    // foo + bar in pool, baz in use

    EXPECT(pool.usage() == 1);
    EXPECT(pool.size() == 2);

    pool.checkin(baz);

    // baz + foo + bar in pool, none in use

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 3);

    DataHandle* marco = pool.checkout("marco.data");
    marco->write(buffer, buffer.size());

    // baz + foo + bar in pool, marco in use

    EXPECT(pool.usage() == 1);
    EXPECT(pool.size() == 3);

    pool.checkin(marco);

    // marco + foo + baz in pool, none in use, bar was purged (closed)

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 3);

    Log::info() << pool << std::endl;

    pool.capacity(1);

    // marco in pool, none in use, foo + baz were purged (closed) due to reduced capacity

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 1);

    Log::info() << pool << std::endl;

    DataHandle* polo = pool.checkout("polo.data");
    polo->write(buffer, buffer.size());

    // marco in pool, polo is in use

    EXPECT(pool.usage() == 1);
    EXPECT(pool.size() == 1);

    Log::info() << pool << std::endl;

    pool.checkin(polo);

    // polo pushes marco out and remains alone in pool

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 1);

    Log::info() << pool << std::endl;

    // remove polo and marco (latter has no effect but is legal)

    EXPECT(pool.remove("polo.data"));
    EXPECT(!pool.remove("marco.data"));

    EXPECT(pool.usage() == 0);
    EXPECT(pool.size() == 0);

    // checking in a never checked out DataHandle throws

    std::unique_ptr<FileHandle> fhp(new FileHandle("foo.data"));
    EXPECT_THROWS_AS(pool.checkin(fhp.get()), eckit::SeriousBug);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    int failures = run_tests(argc, argv);

    for (size_t i = 0; i < 5; ++i) {
        PathName path(eckit::test::files[i]);
        if (path.exists())
            path.unlink();
    }

    return failures;
}
