/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/container/Queue.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::types;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

void mlog(const char* s, int e) {
    static std::mutex mutex;
    std::unique_lock<std::mutex> locker(mutex);
    std::cout << s << e << std::endl;
}

// static void plog(int e) {
//     mlog(" -> ", e);
// }

// static void clog(int e) {
//     mlog(" <- ", e);
// }

//----------------------------------------------------------------------------------------------------------------------

#define MULT 10

CASE("Multi Producer Multi Consumer Queue access size = 1") {

    eckit::Queue<int> q(1);  // queue depth 1 to create high-contention, bug found with nprod = 93 and ncons = 77
    {
        size_t nprod = 13;  // = 93
        size_t ncons = 7;   // = 77

        std::vector<std::thread> producers;
        for (int id = 0; id < nprod; ++id) {
            producers.emplace_back(std::thread([&q, id, ncons] {
                for (int j = 0; j < MULT * ncons; ++j) {
                    int e = 1000 * id + j;
                    q.push(e);
                    // plog(e);
                    std::this_thread::sleep_for(std::chrono::milliseconds(3));
                }
            }));
        }

        std::vector<std::thread> consumers;
        for (int id = 0; id < ncons; ++id) {
            consumers.emplace_back(std::thread([&q, nprod] {
                for (int j = 0; j < MULT * nprod; ++j) {
                    int e;
                    ASSERT(q.pop(e) >= 0);
                    // clog(e);
                    std::this_thread::sleep_for(std::chrono::milliseconds(3));
                }
            }));
        }

        for (auto& p : producers) {
            p.join();
        }

        for (auto& c : consumers) {
            c.join();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
