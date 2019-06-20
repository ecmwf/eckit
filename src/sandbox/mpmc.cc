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

int main() {
    size_t nprod = 19;
    size_t ncons = 23;

    eckit::Queue<int> q(8);

    std::vector<std::thread> producers;
    for (int id = 0; id < nprod; ++id) {
        producers.emplace_back(std::thread([&q, id, ncons] {
            for (int j = 0; j < ncons; ++j) {
                int e = 1000 * id + j;
                q.push(e);
                plog(e);
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }));
    }

    std::vector<std::thread> consumers;
    for (int id = 0; id < ncons; ++id) {
        consumers.emplace_back(std::thread([&q, id, nprod] {
            for (int j = 0; j < nprod; ++j) {
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
