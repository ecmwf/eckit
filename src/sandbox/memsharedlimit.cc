#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cmath>  // for ::lround()
#include <iostream>
#include <limits>
#include <memory>
#include <thread>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/os/SharedInt.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/runtime/Tool.h"

using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

unsigned short nlimit() {

    /// @note We dont use SEM_VALUE_MAX as this can be changed by the operating system configuration

    static long system_semmax = SYSCALL(::sysconf(_SC_SEM_VALUE_MAX));  // guaranteed to be positive

    long semmax = std::min<long>(std::numeric_limits<short>::max(),
                                 system_semmax);  // guaranteed to cast to unsigned short without precision loss

    return (static_cast<unsigned short>(semmax) / 8) * 8;  // max multiple of 8 less than semmax
}

pid_t pid                 = 0;
const unsigned short SLOT = 0;

/// Class used to avoid multiple processes in same system allocating too much memory
/// Processes will sleep until available memory is released
/// We partition the memory space to limit with the precision of a short

class MemorySharedLimit {

public:  // types
    typedef unsigned short parts_t;

    struct Token : private eckit::NonCopyable {

        Token(MemorySharedLimit& limit, parts_t parts) :
            limit_(limit), parts_(parts) {}

        ~Token() { limit_.release(parts_); }

    private:
        MemorySharedLimit& limit_;
        parts_t parts_;
    };

public:  // methods
    MemorySharedLimit(const PathName& path, size_t managed) :
        shint_(path, 1), managed_(managed) {
        ASSERT(short(nlimit()) == nlimit());  // "SEM_MAX_VALUE must fit short in this architecture

        std::cout << "nlimit() " << nlimit() << std::endl;

        int curr = shint_.limit(SLOT);  // limit may have been set by another process
        if (curr != nlimit()) {
            shint_.newLimit(short(nlimit()), SLOT);
        }

        int available = shint_.free(SLOT);
        ASSERT(available <= nlimit());  // if this fails, try to remove the path to reset the semaphore

        partsize_ = managed_ / nlimit();

        std::cout << "part size " << partsize_ << std::endl;

        std::cout << "max managed " << maxManaged() << std::endl;
        std::cout << "free mem " << free() << std::endl;

        std::cout << "MemorySharedLimit managing " << eckit::Bytes(maxManaged()) << " in " << nlimit() << " parts of "
                  << eckit::Bytes(partsize_) << std::endl;

        ASSERT(maxManaged() <= managed_);
    }

    size_t maxManaged() const { return partsize_ * nlimit(); }

    size_t free() { return size_t(shint_.free(SLOT)) * partsize_; }

    /// @pre mem is larger than zero
    std::unique_ptr<MemorySharedLimit::Token> use(size_t mem) {
        ASSERT(mem > 0);

        parts_t parts = nparts(mem);
        if (shint_.free(SLOT) < parts)
            std::cout << pid << " WAIT " << parts << std::endl;

        shint_.use(SLOT, short(parts));  // processes wait here for available parts

        std::cout << pid << " <<< " << parts << " : (" << mem << ")" << std::endl;
        return std::unique_ptr<MemorySharedLimit::Token>(new Token(*this, parts));
    }

    void release(parts_t parts) {
        std::cout << pid << " >>> " << parts << std::endl;
        shint_.release(SLOT, short(parts));
    }

private:  // methods
    parts_t partitions(size_t mem) {
        size_t parts = mem / partsize_;
        return static_cast<parts_t>(parts);
    }

    parts_t nparts(size_t mem) {
        if (mem <= managed_) {
            parts_t parts = partitions(mem);
            ASSERT(parts <= nlimit());
            return parts;
        }

        std::ostringstream oss;
        oss << "Request for too much memory to MemorySharedLimit,"
            << " managing: " << eckit::Bytes(managed_) << " requested: " << eckit::Bytes(mem);
        throw eckit::BadValue(oss.str(), Here());
    }

private:                      // members
    eckit::SharedInt shint_;  ///< shared int
    size_t managed_;          ///< amount of memory managed
    size_t partsize_;         ///< minimal amount of memory possible to check out
};


//----------------------------------------------------------------------------------------------------------------------

const size_t MEMMAX = 8 * 1024 * 1024;

class Forker : public ProcessControler {

    void run() {
        ::srandom(unsigned(::getpid()) + unsigned(::time(nullptr)));

        pid = ::getpid();

        std::cout << "process " << pid << std::endl;

        MemorySharedLimit memresource("~/locks/maxbuff", MEMMAX);

        for (;;) {

            size_t mem = 1024 * 1024 * (1 + ::lround(5 * double(::random()) / double(RAND_MAX)));

            //            std::cout << pid << " >>> (" << mem << ")" << std::endl;

            std::unique_ptr<MemorySharedLimit::Token> token = memresource.use(mem);

            eckit::Buffer buffer(mem);

            double ms = 1000 * (double(::random()) / double(RAND_MAX));

            std::this_thread::sleep_for(std::chrono::milliseconds(int(ms)));
        }
    }

public:
    Forker() :
        ProcessControler(true) {}
};


//----------------------------------------------------------------------------------------------------------------------

const short nprocs = 8;

class MemApp : public Tool {
public:
    MemApp(int argc, char** argv) :
        Tool(argc, argv, "HOME") {}

    ~MemApp() {}

private:
    MemApp(const MemApp&) = delete;
    MemApp& operator=(const MemApp&) = delete;

    virtual void run() {

        std::cout << "forking processes" << std::endl;

        for (short i = 1; i <= nprocs; ++i) {
            try {
                Forker f;
                f.start();
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }

        {
            MemorySharedLimit memresource("~/locks/maxbuff", MEMMAX);
            for (;;) {
                ::sleep(1);
                std::cout << "FREE MEM " << /*eckit::Bytes*/ (memresource.free()) << std::endl;
            }
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------1

int main(int argc, char** argv) {
    MemApp app(argc, argv);
    app.start();
    return 0;
}
