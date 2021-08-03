/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/Mutex.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

class Locker : public eckit::Thread {
public:
    Locker(Mutex* m, bool& v) :
        m_(m), v_(v) {}
    Mutex* m_;
    bool& v_;
    void run() { v_ = m_->tryLock(); }
};

CASE("Mutex Lock/Unlock") {

    Mutex* m;

    EXPECT_NO_THROW(m = new Mutex());

    bool gotLock = false;
    EXPECT_NO_THROW(gotLock = m->tryLock());
    EXPECT(gotLock);
    EXPECT_NO_THROW(m->unlock());

    EXPECT_NO_THROW(m->lock());

    ThreadControler thread(new Locker(m, gotLock), false);
    thread.start();
    EXPECT_NO_THROW(thread.wait());

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
