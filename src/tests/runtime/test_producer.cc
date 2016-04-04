/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/runtime/Tool.h"
#include "eckit/runtime/ProducerConsumer.h"
#include "eckit/utils/Translator.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

    class TestProducer : public Tool {
        virtual void run();

    public:

        TestProducer(int argc, char** argv): Tool(argc,argv) {}

    };


    struct C : public Consumer<string> {

        virtual void consume(string& s) {
            Log::info() << "Consume " << s << std::endl;
            ::usleep(10000);
        }

    };

    struct P : public Producer<string> {

        int count_;

        virtual bool done() {
            return count_ <= 0;
        }

        virtual void produce(string& s) {
            Log::info() << "Produce " << count_ << std::endl;
            ::usleep(count_*10000);
            ASSERT(count_); s = string("Hello, world! ") + Translator<int,string>()(count_); count_--;
        }

        P() : count_(5) {}
    };

    void TestProducer::run()
    {
        P p;
        C c;
        ProducerConsumer<string> pc;
        pc.execute(p, c);
    }

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestProducer app(argc,argv);
    return app.start();
}
