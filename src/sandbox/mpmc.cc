#include <chrono>
#include <thread>

#include "eckit/container/Queue.h"

void mlog(const char* s, int e) {
    static std::mutex mutex;
    std::unique_lock<std::mutex> locker(mutex);
    std::cout << s << e << std::endl;
}

void plog(int e) {
    mlog(" -> ", e);
}

void clog(int e) {
    mlog(" <- ", e);
}

#define MULT 100

int main() {
    size_t nprod = 93;
    size_t ncons = 77;

    eckit::Queue<int> q(1);

    std::vector<std::thread> producers;
    for (int id = 0; id < nprod; ++id) {
        producers.emplace_back(std::thread([&q, id, ncons] {
            for (int j = 0; j < MULT * ncons; ++j) {
                int e = 1000 * id + j;
                q.push(e);
                plog(e);
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
                clog(e);
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
