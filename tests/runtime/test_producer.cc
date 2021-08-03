/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/runtime/ProducerConsumer.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Translator.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {


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

    P() :
        count_(5) {}
};

CASE("test_producer") {
    P p;
    C c;
    ProducerConsumer<string> pc;
    pc.execute(p, c);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
