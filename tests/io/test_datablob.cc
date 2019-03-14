/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "eckit/io/DataBlob.h"
#include "eckit/io/DataHandle.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/types/Metadata.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

namespace {

    // A trivial do-nothing metadata object
    class TestMetadata : public Metadata {
    public: // methods
        TestMetadata() {}
        virtual ~TestMetadata() {}
        virtual std::vector<std::string> keywords() const { NOTIMP; };
        virtual bool has(const std::string& name) const { NOTIMP; }
        virtual void get(const std::string& name, std::string& value) const { NOTIMP; }
        virtual void get(const std::string& name, long& value) const { NOTIMP; }
        virtual void get(const std::string& name, double& value) const { NOTIMP; }
        friend std::ostream& operator<<(std::ostream& s, const TestMetadata& p) {
            p.print(s);
            return s;
        }
    protected: // methods
        virtual void print(std::ostream& os) const { os << "TestMetadata()"; }
    };

    //
    // A null datablob for testing the factories
    class TestDataBlob : public DataBlob {

    public: // methods

        TestDataBlob(const void* data, size_t length) : DataBlob(data, length) {}
        TestDataBlob(DataHandle& dh, size_t length) : DataBlob(dh, length) {}

        virtual ~TestDataBlob() {}

        virtual const Metadata& metadata() const { return metadata_; }

    private: // methods

        virtual void print(std::ostream& os) const { os << "TestDataBlob()"; }

    private: // members

        TestMetadata metadata_;
    };

    DataBlobBuilder<TestDataBlob> dbBuilder("test");
}

// ----------------------------------------------------------------------------------------

CASE ( "test_eckit_io_datablob_factory_generate" )
{
    ScopedPtr<DataBlob> blob(DataBlobFactory::build("test", NULL, 0));

    // Check that we generate a blob of the correct type (and implicitly that the factory
    // is correctly registered).
    TestDataBlob * testBlob = dynamic_cast<TestDataBlob*>(blob.get());
    EXPECT( testBlob );
}

CASE ( "test_eckit_io_datablob_factory_list" )
{
    // DataBlobFactory::list appends the results to a ostream&, so we need to extract them.
    std::stringstream ss;
    DataBlobFactory::list(ss);

    // Extract the seperate components from the string stream into a vector
    std::vector<std::string> bits;
    Tokenizer(" ,")(ss.str(), bits);

    // We expect the file and MultIO factories to be in there too...
    EXPECT ( std::find( bits.begin(), bits.end(), "json" ) != bits.end() );
    EXPECT ( std::find( bits.begin(), bits.end(), "test" ) != bits.end() );
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main(int argc,char **argv)
{
    return run_tests ( argc, argv );
}

