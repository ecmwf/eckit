/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Application.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/ThreadPool.h"

using namespace eckit;

class Test : public Application {
    virtual void run();

public:
    Test(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};

class X : public ThreadPoolTask {
    int n_;
    void execute() {
        std::cout << "ThreadPoolTask " << n_ << std::endl;
        sleep(n_ % 10);

        ASSERT(n_ != 42);
    }

public:
    X(int n) :
        n_(n) {}
};

void Test::run() {
    ThreadPool pool("p", 20);
    for (int i = 0; i < 100; i++)
        pool.push(new X(i));

    // pool.waitForThreads();
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
