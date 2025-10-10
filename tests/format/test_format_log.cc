/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#include <eckit/testing/Test.h>

#include "eckit/log/Channel.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/system/Library.h"
#include "stdlib.h"


class MyTestLib : public eckit::system::Library {
public:

    MyTestLib() :
        eckit::system::Library(([]() {
            setenv("MY_TEST_LIB_DEBUG", "1", 1);
            return std::string("my-test-lib");
        })()),
        channel{new eckit::OStreamTarget{oss}} {};

    static const MyTestLib& instance() {
        static MyTestLib mytestlib;
        return mytestlib;
    };

    // Public oss to log to
    std::ostringstream oss;
    mutable eckit::Channel channel;

    eckit::Channel& debugChannel() const override { return channel; };

protected:

    const void* addr() const override { return this; }
    std::string version() const override { return "123"; }
    std::string gitsha1(unsigned int count) const override { return "not a sha"; };
};

REGISTER_LIBRARY(MyTestLib);


CASE("Can use FMT_DEBUG_LIB  macro") {
    auto& lib = MyTestLib::instance();
    DEBUG_MSG(MyTestLib, "Debug log {} {}", 1, 1.123);
    lib.channel.flush();
    EXPECT_EQUAL(lib.oss.str(), std::string("Debug log 1 1.123"));
}


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
