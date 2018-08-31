#include <unistd.h>
#include <semaphore.h>

#include <iostream>
#include <chrono>
#include <thread>

#include "eckit/os/SharedInt.h"
#include "eckit/runtime/Tool.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/filesystem/PathName.h"

using namespace eckit;

#define SLOT 0

//--------------------------------------------------------------------------------------------------

class SemForker : public ProcessControler {


    void run()
    {
        sem_t *sem;

        ::srandom(::getpid() + ::time(0));

        pid_t pid = ::getpid();

        std::cout << "process " << pid << std::endl;

        eckit::SharedInt shint(PathName("~/locks/maxbuff"), 1);

        for(;;) {

            if(shint.free(SLOT) < bs_) {
                std::cout << pid << " -- not enough resources (" << bs_ << ")" << std::endl;
            }
            shint.use(SLOT, bs_);
            std::cout << pid << " >>> (" << bs_ << ")" << std::endl;

            double ms = 1000*(double(::random()) / double(RAND_MAX));

//            for(;;) { ::sleep(1); }

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms)));

            shint.release(SLOT, bs_);
            std::cout << pid << " <<< (" << bs_ << ")" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms/3)));
        }
    }

public:

    SemForker(short bs) : ProcessControler(true), bs_(bs) {
    }

    short bs_;
};


//--------------------------------------------------------------------------------------------------

class SemApp : public Tool {
public:

    SemApp(int argc, char** argv) :
        Tool(argc, argv, "HOME")
    {}

    ~SemApp() {}

private:

    short limits(short p) {
        short lim = 0;
        for(short i = 1; i <=p; ++i) {
            lim += i;
        }
        return lim;
    }


    SemApp(const SemApp&) = delete;
    SemApp& operator=(const SemApp&) = delete;

    virtual void run() {

        const short nprocs = 4;
        const short limit = limits(nprocs) + 8;


        std::cout << "init main process" << std::endl;
        {
            eckit::SharedInt shint(PathName("~/locks/maxbuff"), 1);
            int curr = shint.limit(SLOT);
            if(curr != limit) {
                std::cout << "setting limit to " << limit  << std::endl;
                shint.newLimit(limit, SLOT);
            }
            else {
                std::cout << "limit is " << curr  << std::endl;
            }
            std::cout << "FREE " << shint.free(SLOT) << std::endl;
        }

        std::cout << "forking processes" << std::endl;

        for(short i = 1; i <= nprocs; ++i) {
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
            eckit::SharedInt shint(PathName("~/locks/maxbuff"), 1);
            for(;;) {
                ::sleep(1);
                std::cout << shint.free(SLOT) << std::endl;
            }
        }
    }
};

//--------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    SemApp app(argc, argv);
    app.start();
    return 0;
}
