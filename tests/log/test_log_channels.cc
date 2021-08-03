/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Tool.h"

#include "eckit/log/CallbackTarget.h"
#include "eckit/log/Channel.h"
#include "eckit/log/ColouringTarget.h"
#include "eckit/log/FileTarget.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/WrapperTarget.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


//----------------------------------------------------------------------------------------------------------------------

#if 1
#define DEBUG_H
#define DEBUG_(x)
#else
#define DEBUG_H std::cerr << " DEBUG @ " << __FILE__ << " +" << __LINE__ << std::endl;
#define DEBUG_(x) std::cerr << #x << " : [" << x << "] @ " << __FILE__ << " +" << __LINE__ << std::endl;
#endif

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

/// Example of a wrapper target that capitalizes all output

class CapitalizerTarget : public WrapperTarget {
public:
    CapitalizerTarget(LogTarget* target) :
        WrapperTarget(target) {}

private:
    virtual void write(const char* start, const char* end) {

        std::string::size_type length = std::distance(start, end);
        buffer_.resize(length);

        std::locale loc;
        const char* p = start;
        for (std::string::size_type i = 0; i < length; ++i, ++p) {
            buffer_[i] = std::toupper(*p, loc);
        }

        target_->write(buffer_.c_str(), buffer_.c_str() + length);
    }

    virtual void writePrefix() {}
    virtual void writeSuffix() {}

    void print(std::ostream& s) const { s << "CapitalizerTarget()"; }

    std::string buffer_;
};

//----------------------------------------------------------------------------------------------------------------------

static void callback_ctxt(void* ctxt, const char* msg) {
    std::cout << "[" << *((int*)ctxt) << "] : -- " << msg << std::endl;
}

static void callback_noctxt(void*, const char* msg) {
    std::cout << "[CALLBACK OUT] : -- " << msg << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------


CASE("test_multi_targets") {

    std::cout << "---> test_multi_targets()" << std::endl;

    int t = 0;

    Channel mychannel;

    mychannel << "testing [" << t++ << "]" << std::endl;

    mychannel.addFile("test.txt");

    mychannel << "testing [" << t++ << "]" << std::endl;

    std::ofstream of("test.txt.2");
    mychannel.addStream(of);

    mychannel << "testing [" << t++ << "]" << std::endl;

    mychannel.addStream(std::cout);

    mychannel << "testing [" << t++ << "]" << std::endl;

    mychannel.addStream(std::cerr);

    mychannel << "testing [" << t++ << "]" << std::endl;

    std::ostringstream oss;
    mychannel.addStream(oss);

    mychannel << "testing [" << t++ << "]" << std::endl;

    mychannel.addCallback(&callback_noctxt, 0);
    mychannel.addCallback(&callback_ctxt, &t);

    mychannel << "testing [" << t++ << "]" << std::endl;

    // mychannel.addLogTarget(new CapitalizerTarget(new FileTarget(PathName("capitals.txt"))));

    mychannel << "testing [" << t++ << "]" << std::endl;

    mychannel << "Final test" << std::endl;
}

CASE("test_multi_colouring") {
#if 0
    Log::info().setLogTarget(    new ColouringTarget(new OStreamTarget(std::cout), &Colour::green));
    Log::warning().setLogTarget( new ColouringTarget(new OStreamTarget(std::cerr), &Colour::yellow));
    Log::error().setLogTarget(   new ColouringTarget(new OStreamTarget(std::cerr), &Colour::red));

    Log::info()    << "Log::info() is green"     << std::endl;
    Log::warning() << "Log::warning() is yellow" << std::endl;
    Log::error()   << "Log::error() is red"      << std::endl;
#endif
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

void on_signal_dumpbacktrace(int signum) {
    printf("Caught signal %d\n", signum);
    std::cerr << BackTrace::dump() << std::endl;
    eckit::LibEcKit::instance().abort();
}

int main(int argc, char** argv) {
    signal(SIGSEGV, on_signal_dumpbacktrace);

    return run_tests(argc, argv);
}
