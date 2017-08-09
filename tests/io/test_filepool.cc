/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/FilePool.h"
#include "eckit/thread/ThreadPool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

static const size_t BUF_SIZE = 1024;

static const char* files[] = {"foo.data", "bar.data", "baz.data", "marco.data", "polo.data"};

class FilePoolUser : public ThreadPoolTask {
public:
    FilePoolUser(FilePool& pool, int id) : pool_(pool), id_(id) {}
private:
    virtual void execute() {
        std::vector<char> buffer(BUF_SIZE, id_);
        DataHandle* foo = pool_.checkout("foo.data");
        foo->write(&buffer[0], buffer.size());
        pool_.checkin(foo);
    }

    FilePool& pool_;
    int id_;
};

//----------------------------------------------------------------------------------------------------------------------

CASE ( "test_eckit_io_filepool_threads" ) {
    const size_t nThreads = Resource<size_t>("$ECKIT_TEST_THREADS", 16);

    ThreadPool threads("filepool", nThreads);
    FilePool pool(1);
    for (size_t i = 0; i < nThreads; ++i) {
        threads.push(new FilePoolUser(pool, i + 1));
    }
    threads.waitForThreads();

    DataHandle* foo = pool.checkout("foo.data");
    EXPECT( foo->openForRead() >= Length(nThreads * BUF_SIZE) );

    // Check we have nThreads blocks of BUF_SIZE with Bytes 1 to nThreads
    std::vector<bool> found(nThreads);
    for (size_t i = 0; i < nThreads; ++i) {
        std::vector<char> buffer(BUF_SIZE);
        foo->read(&buffer[0], BUF_SIZE);
        const char c = buffer[0];
        EXPECT( c > 0 );
        EXPECT( !found[c-1] );
        std::vector<char> expect(BUF_SIZE, c);
        EXPECT( buffer == expect );
        found[c-1] = true;
    }
    for (size_t i = 0; i < nThreads; ++i) {
        EXPECT( found[i] );
    }

    pool.checkin(foo);
    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size() == 1 );
}

CASE ( "test_eckit_io_filepool_0" ) {
    Buffer buffer(BUF_SIZE);
    ::memset(buffer, 0, buffer.size());

    FilePool pool(1);

    // pool sized 1

    EXPECT( pool.capacity() == 1 );
    EXPECT( pool.size() ==     0 );

    pool.capacity(3);

    // pool is larger

    EXPECT( pool.capacity() == 3 );
    EXPECT( pool.size() == 0 );

    DataHandle* foo = pool.checkout("foo.data");
    foo->write(buffer, buffer.size());

    // foo is in use

    EXPECT( pool.usage() == 1 );
    EXPECT( pool.size()  == 0 );

    DataHandle* bar = pool.checkout("bar.data");
    bar->write(buffer, buffer.size());

    // foo + bar are in use

    EXPECT( pool.usage() == 2 );
    EXPECT( pool.size()  == 0 );

    Log::info() << pool << std::endl;

    pool.checkin(foo);
    pool.checkin(bar);

    Log::info() << pool << std::endl;

    // foo + bar are in pool, none in use

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 2 );

    DataHandle* foo2 = pool.checkout("foo.data");
    foo2->write(buffer, buffer.size());

    // foo in use again, bar in pool

    EXPECT( pool.usage() == 1 );
    EXPECT( pool.size()  == 1 );

    pool.checkin(foo);

    // foo + bar are again in pool, none in use

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 2 );

    DataHandle* baz = pool.checkout("baz.data");
    baz->write(buffer, buffer.size());

    // foo + bar in pool, baz in use

    EXPECT( pool.usage() == 1 );
    EXPECT( pool.size()  == 2 );

    pool.checkin(baz);

    // baz + foo + bar in pool, none in use

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 3 );

    DataHandle* marco = pool.checkout("marco.data");
    marco->write(buffer, buffer.size());

    // baz + foo + bar in pool, marco in use

    EXPECT( pool.usage() == 1 );
    EXPECT( pool.size()  == 3 );

    pool.checkin(marco);

    // marco + foo + baz in pool, none in use, bar was purged (closed)

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 3 );

    Log::info() << pool << std::endl;

    pool.capacity(1);

    // marco in pool, none in use, foo + baz were purged (closed) due to reduced capacity

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 1 );

    Log::info() << pool << std::endl;

    DataHandle* polo = pool.checkout("polo.data");
    polo->write(buffer, buffer.size());

    // marco in pool, polo is in use

    EXPECT( pool.usage() == 1 );
    EXPECT( pool.size()  == 1 );

    Log::info() << pool << std::endl;

    pool.checkin(polo);

    // polo pushes marco out and remains alone in pool

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 1 );

    Log::info() << pool << std::endl;

    // remove polo and marco (latter has no effect but is legal)

    EXPECT( pool.remove("polo.data") );
    EXPECT( !pool.remove("marco.data") );

    EXPECT( pool.usage() == 0 );
    EXPECT( pool.size()  == 0 );

    // checking in a never checked out DataHandle throws

    EXPECT_THROWS_AS( pool.checkin(new FileHandle("foo.data")), eckit::SeriousBug );
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main(int argc,char **argv)
{
    int failures = run_tests ( argc, argv );

    for (size_t i = 0; i < 5; ++i) {
        PathName path(eckit::test::files[i]);
        if (path.exists()) path.unlink();
    }
    
    return failures;
}


