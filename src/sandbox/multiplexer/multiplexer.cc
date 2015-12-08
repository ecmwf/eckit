/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/config/Resource.h"

#include "eckit/runtime/Tool.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/io/Buffer.h"

#include "sandbox/multiplexer/DataSink.h"
#include "sandbox/multiplexer/DummySource.h"

namespace eckit {
namespace multiplexer {

//----------------------------------------------------------------------------------------------------------------------

class Multiplexer : public Tool {
public:

    Multiplexer(int argc,char **argv): Tool(argc,argv) {}

    ~Multiplexer() {}

    virtual void run();
};

void Multiplexer::run()
{
    DummySource source("foo", "twiddle.tmp");
    source.generate_field();

    eckit::ScopedPtr<DataSink> ptr( DataSinkFactory::build("foo") );

    Buffer buffer(1024);

    ptr->open("lolo.txt");
    ptr->write(buffer, buffer.size());
    ptr->close();
}

} // namespace multiplexer
} // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

using namespace eckit::multiplexer;

int main(int argc,char **argv)
{
    Multiplexer app(argc,argv);
    app.start();
    return 0;
}

