/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_user_channels

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/LocalPathName.h"

#include "eckit/log/Channel.h"
#include "eckit/log/MultiChannel.h"
#include "eckit/log/CallbackChannel.h"
#include "eckit/log/FileChannel.h"
#include "eckit/log/FormatChannel.h"
#include "eckit/log/FormatBuffer.h"
#include "eckit/log/ColorizeFormat.h"
#include "eckit/log/ChannelBuffer.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Example of a third-party buffer
/// To use with Channel
class ForwardBuffer : public ChannelBuffer {
public:
    ForwardBuffer(std::ostream* os) : ChannelBuffer(os) {}

    ForwardBuffer(std::ostream& os) : ChannelBuffer(os) {}

protected:
    virtual bool dumpBuffer() {
        os_->write(pbase(), pptr() - pbase());
        setp(pbase(), epptr());
        return true;
    }
};

//-----------------------------------------------------------------------------

/// Example of a formating channel
class Spacer : public FormatBuffer {
public:
    Spacer(std::size_t size = 1024) : FormatBuffer(0, size) {}

    virtual ~Spacer() { pubsync(); }

    virtual void process(const char* begin, const char* end) {
        const char* p = begin;
        while (p != end) {
            *target() << *p << "_";
            ++p;
        }
    }
};

//-----------------------------------------------------------------------------

static void callback_ctxt(void* ctxt, const char* msg) {
    std::cout << "[" << *((int*)ctxt) << "] : -- " << msg << std::endl;
}

static void callback_noctxt(void*, const char* msg) {
    std::cout << "[CALLBACK OUT] : -- " << msg << std::endl;
}

//-----------------------------------------------------------------------------

}  // namespace eckit_test

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(test_eckit_user_log_channels)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_eckit_user_log_channel_registration) {
    using namespace eckit_test;

    int t = 0;

    // delcare a new multichannel
    MultiChannel* mc = new MultiChannel;
    BOOST_CHECK(mc);

    FileChannel* fc = new FileChannel(LocalPathName("test.txt"));
    BOOST_CHECK(fc);

    mc->add("file", fc);

    std::ofstream of("test.txt.2");
    mc->add("of", new Channel(new ForwardBuffer(of)));

    std::ofstream of3("test.txt.3");
    mc->add("of3", of3);
    mc->add("cout", std::cout);
    mc->add("fwd_cout", new Channel(new ForwardBuffer(std::cout)));

    std::ostringstream oss;
    mc->add("oss", new Channel(new ForwardBuffer(oss)));

    CallbackChannel* cbc = new CallbackChannel();
    BOOST_CHECK(cbc);

    cbc->register_callback(&callback_noctxt, 0);
    mc->add("cbc", cbc);

    CallbackChannel* cbw = new CallbackChannel();
    BOOST_CHECK(cbw);

    cbw->register_callback(&callback_ctxt, &t);
    mc->add("cbw", cbw);
    mc->add("fc", new FormatChannel(std::cerr, new Spacer()));

    ColorizeFormat* cfb = new ColorizeFormat();
    BOOST_CHECK(cfb);
    cfb->setColor(&Colour::red);
    cfb->resetColor(&Colour::reset);
    mc->add("cc", new FormatChannel(new FormatChannel(std::cerr, cfb), new Spacer()));

    // register the new multichannel
    Log::registerChannel("multi_channel", mc);

    // test the registered channel
    Log::channel("multi_channel") << "TEST OF MULTI-CHANNEL" << std::endl;
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
