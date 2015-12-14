/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#define BOOST_TEST_MODULE test_eckit_sandbox_multiplexer

#include "ecbuild/boost_test_framework.h"

#include "eckit/io/Length.h"
#include "eckit/memory/ScopedPtr.h"
#include "sandbox/multiplexer/DataSink.h"
#include "sandbox/multiplexer/DummySink.h"

using namespace std;
using namespace eckit;
using namespace eckit::multiplexer;

// --------------------------------------------------------------------------------

namespace eckit_test {

//
// A test-dummy sink.
class TestDummySink : public DataSink {

public:
    virtual void open(const std::string& key) {}
    virtual void write(const void * buffer, const Length& length) {}
    virtual void close() {}
    virtual bool is_open() const {}

protected:
    virtual void print(std::ostream&) const {}

};

// Ensure this is registered with the factory
DataSinkBuilder<TestDummySink> testDummyBuilder("test-dummy");

// --------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(test_eckit_sandbox_multiplexer)

BOOST_AUTO_TEST_CASE(test_multiplexer_factory)
{
    ScopedPtr<DataSink> ptr(DataSinkFactory::build("test-dummy"));

    // Check that the factory returns an object of the correct type
    TestDummySink * ptmp = dynamic_cast<TestDummySink*>(ptr.get());
    BOOST_CHECK_EQUAL(ptmp, ptr.get());
}

BOOST_AUTO_TEST_SUITE_END()

// --------------------------------------------------------------------------------

} // namespace eckit_test
