/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <future>

#include "eckit/thread/Mutex.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("TestMutex") {
    Mutex* m = 0;

    EXPECT_NO_THROW(m = new Mutex());

    bool gotLock = false;
    EXPECT_NO_THROW(gotLock = m->tryLock());
    EXPECT(gotLock);
    EXPECT_NO_THROW(m->unlock());

    EXPECT_NO_THROW(m->lock());

    // Mutex is recursive, therefore another thread is needed for tryLock to return false
    auto f_gotLock = async(launch::async, [&m] () {
            return m->tryLock();
        });
    EXPECT_NO_THROW(gotLock = f_gotLock.get());
    EXPECT(!gotLock);

    EXPECT_NO_THROW(m->unlock());

    delete m;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
