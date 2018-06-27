#include <unistd.h>

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
        ::srand(::getpid() + ::time(0));
        ::srandom(::getpid() + ::time(0));

        pid_t pid = ::getpid();

        std::cout << "process " << pid << std::endl;

        eckit::SharedInt shint(PathName("~/locks/maxbuff"), 1);

        for(;;) {

            if(shint.free(SLOT) == 0) {
                std::cout << pid << " -- no free resources" << std::endl;
            }
            shint.use(SLOT);
            std::cout << pid << " >>>" << std::endl;

            double ms = 1000*(double(::random()) / double(RAND_MAX));

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms)));

            shint.release(SLOT);
            std::cout << pid << " <<<" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms/3)));
        }
    }

public:

    SemForker() : ProcessControler(true) {
    }
};


//--------------------------------------------------------------------------------------------------

const int limit = 20;

class SemApp : public Tool {
public:

    SemApp(int argc, char** argv) :
        Tool(argc, argv, "HOME")
    {}

    ~SemApp() {}

private:

    SemApp(const SemApp&) = delete;
    SemApp& operator=(const SemApp&) = delete;

    virtual void run() {

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
        }

        std::cout << "forking processes" << std::endl;

        for(int i = 0; i < 10; ++i) {
            try {
                SemForker f;
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
