// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/testing/Test.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

class Locker : public eckit::Thread {
public:

    Locker(Mutex* m, bool& v) : m_(m), v_(v) {}
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

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
