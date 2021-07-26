#include <semaphore.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <thread>

#include "eckit/filesystem/PathName.h"
#include "eckit/os/SharedInt.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/runtime/Tool.h"

using namespace eckit;

#define SLOT 0

//----------------------------------------------------------------------------------------------------------------------

class SemForker : public ProcessControler {

    void run() {
        ::srandom(unsigned(::getpid()) + unsigned(::time(nullptr)));

        pid_t pid = ::getpid();

        std::cout << "process " << pid << std::endl;

        eckit::SharedInt shint(PathName("~/locks/maxbuff"), 1);

        for (;;) {

            int available = shint.free(SLOT);
            if (available < want_) {
                std::cout << pid << " -- not enough resources want: " << want_ << " available: " << available
                          << std::endl;
            }
            shint.use(SLOT, want_);
            std::cout << pid << " >>> (" << want_ << ")" << std::endl;

            double ms = 1000 * (double(::random()) / double(RAND_MAX));

            //            for(;;) { ::sleep(1); }

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms)));

            shint.release(SLOT, want_);
            std::cout << pid << " <<< (" << want_ << ")" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms / 3)));
        }
    }

public:
    SemForker(short want) :
        ProcessControler(true), want_(want * 500) {}

    short want_;
};


//----------------------------------------------------------------------------------------------------------------------

const short nprocs = 16;

class SemApp : public Tool {
public:
    SemApp(int argc, char** argv) :
        Tool(argc, argv, "HOME") {}

    ~SemApp() {}

private:
    short limits() {
        //        short lim = 0;
        //        for(short i = 1; i <= nprocs; ++i) {
        //            lim += i;
        //        }
        //        return lim + 8;

        return std::numeric_limits<short>::max();
    }

    SemApp(const SemApp&) = delete;
    SemApp& operator=(const SemApp&) = delete;

    virtual void run() {

        const short limit    = limits();
        const int sharedints = 1;
        const PathName path  = "~/locks/maxbuff";

        std::cout << "init main process" << std::endl;
        {
            eckit::SharedInt shint(path, sharedints);
            int curr = shint.limit(SLOT);
            if (curr != limit) {
                std::cout << "setting limit to " << limit << std::endl;
                shint.newLimit(limit, SLOT);
            }
            else {
                std::cout << "limit is " << curr << std::endl;
            }
            std::cout << "FREE " << shint.free(SLOT) << std::endl;
        }

        std::cout << "forking processes" << std::endl;

        for (short i = 1; i <= nprocs; ++i) {
            try {
                SemForker f(i);
                f.start();
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }

        {
            eckit::SharedInt shint(path, sharedints);
            for (;;) {
                ::sleep(1);
                std::cout << "FREE " << shint.free(SLOT) << std::endl;
            }
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------1

int main(int argc, char** argv) {
    SemApp app(argc, argv);
    app.start();
    return 0;
}
