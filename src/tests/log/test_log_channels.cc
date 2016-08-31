/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <csignal>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <locale>
#include <fstream>
#include <cassert>

#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Tool.h"
#include "eckit/filesystem/LocalPathName.h"

#include "eckit/log/Channel.h"
#include "eckit/log/ColouringTarget.h"
#include "eckit/log/FileTarget.h"
#include "eckit/log/CallbackTarget.h"
#include "eckit/log/WrapperTarget.h"
#include "eckit/log/OStreamTarget.h"

using namespace std;
using namespace eckit;


#if 1
    #define DEBUG_H
    #define DEBUG_(x)
#else
    #define DEBUG_H     std::cerr << " DEBUG @ " << __FILE__ << " +" << __LINE__ << std::endl;
    #define DEBUG_(x)   std::cerr << #x << " : [" << x << "] @ " <<  __FILE__ << " +" << __LINE__ << std::endl;
#endif

namespace eckit_test {

//----------------------------------------------------------------------------------------------------------------------

/// Example of a wrapper target that capitalizes all output

class CapitalizerTarget : public WrapperTarget {
public:

    CapitalizerTarget(LogTarget* target) : WrapperTarget(target) {}

private:

    virtual void write(const char* start, const char* end) {

        std::string::size_type length = std::distance(start, end);
        buffer_.resize(length);

        std::locale loc;
        const char* p = start;
        for (std::string::size_type i = 0; i< length; ++i, ++p) {
          buffer_[i] = std::toupper(*p,loc);
        }

        target_->write(buffer_.c_str(), buffer_.c_str() + length);
    }

    virtual void writePrefix(){}
    virtual void writeSuffix(){}

    std::string buffer_;

};

//----------------------------------------------------------------------------------------------------------------------

static void callback_ctxt( void* ctxt, const char* msg )
{
    std::cout << "[" << *((int*)ctxt) << "] : -- " << msg << std::endl ;
}

static void callback_noctxt( void* , const char* msg )
{
    std::cout << "[CALLBACK OUT] : -- " << msg << std::endl ;
}

//----------------------------------------------------------------------------------------------------------------------

class TestApp : public Tool {
public:

    TestApp(int argc,char **argv) : Tool(argc,argv) {}

    ~TestApp() {}

    virtual void run()
    {
        test_multi_targets();
        test_multi_colouring();
    }

    void test_multi_targets() {
#if 0
        std::cout << "---> test_multi_targets()" << std::endl;

        int t = 0;

        Channel mychannel;

        mychannel << "testing [" << t++ << "]" << std::endl;

        mychannel.addLogTarget(new FileTarget(PathName("test.txt")));

        mychannel << "testing [" << t++ << "]" << std::endl;

        std::ofstream of ("test.txt.2");
        mychannel.addLogTarget(new OStreamTarget(of));

        mychannel << "testing [" << t++ << "]" << std::endl;

        mychannel.addLogTarget(new OStreamTarget(std::cout));

        mychannel << "testing [" << t++ << "]" << std::endl;

        mychannel.addLogTarget(new OStreamTarget(std::cerr));

        mychannel << "testing [" << t++ << "]" << std::endl;

        std::ostringstream oss;
        mychannel.addLogTarget(new OStreamTarget(oss));

        mychannel << "testing [" << t++ << "]" << std::endl;

        mychannel.addLogTarget(new CallbackTarget(&callback_noctxt,0));
        mychannel.addLogTarget(new CallbackTarget(&callback_ctxt, &t));

        mychannel << "testing [" << t++ << "]" << std::endl;

        mychannel.addLogTarget(new CapitalizerTarget(new FileTarget(PathName("capitals.txt"))));

        mychannel << "testing [" << t++ << "]" << std::endl;

        mychannel << "Final test" << std::endl;
#endif
    }

    void test_multi_colouring() {
#if 0
        Log::info().setLogTarget(    new ColouringTarget(new OStreamTarget(std::cout), &Colour::green));
        Log::warning().setLogTarget( new ColouringTarget(new OStreamTarget(std::cerr), &Colour::yellow));
        Log::error().setLogTarget(   new ColouringTarget(new OStreamTarget(std::cerr), &Colour::red));

        Log::info()    << "Log::info() is green"     << std::endl;
        Log::warning() << "Log::warning() is yellow" << std::endl;
        Log::error()   << "Log::error() is red"      << std::endl;
#endif
    }
};

} // namespace eckit_test

//----------------------------------------------------------------------------------------------------------------------

void on_signal_dumpbacktrace(int signum)
{
    printf("Caught signal %d\n",signum);
    std::cerr << BackTrace::dump() << std::endl;
    ::abort();
}

int main(int argc,char **argv)
{
    signal(SIGSEGV, on_signal_dumpbacktrace );

    eckit_test::TestApp app(argc,argv);
    return app.start();
}

