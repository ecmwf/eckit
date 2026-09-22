// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

#include "eckit/runtime/ProducerConsumer.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/Translator.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

struct C : public Consumer<string> {

    virtual void consume(string& s) {
        Log::info() << "Consume " << s << std::endl;
        ::usleep(10000);
    }
};

struct P : public Producer<string> {

    int count_;

    virtual bool done() { return count_ <= 0; }

    virtual void produce(string& s) {
        Log::info() << "Produce " << count_ << std::endl;
        ::usleep(count_ * 10000);
        EXPECT(count_);
        s = string("Hello, world! ") + Translator<int, string>()(count_);
        count_--;
    }

    P() : count_(5) {}
};

CASE("test_producer") {
    P p;
    C c;
    ProducerConsumer<string> pc;
    pc.execute(p, c);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
