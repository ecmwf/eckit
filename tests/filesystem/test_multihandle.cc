/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/PartFileHandle.h"

#include "eckit/io/Buffer.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

class TestMHHandle  {
public:
    void setup();
    void teardown();
    void test_write();
    PathName path1_;
    PathName path2_;
    PathName path3_;

};

void TestMHHandle::test_write()
{
    const char buf1[] = "abcdefghijklmnopqrstuvwxyz";
    const char buf2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char expect[26 * 2];

    // setformat(std::cout, Log::fullFormat);

    {
        FileHandle f1(path1_);
        f1.openForWrite(0);
        f1.write(buf1, sizeof(buf1));
        f1.close();

        std::cout << path1_ << std::endl;
    }

     {
        FileHandle f2(path2_);
        f2.openForWrite(0);
        f2.write(buf2, sizeof(buf2));
        f2.close();

        std::cout << path2_ << std::endl;

    }

    {
        MultiHandle mh1;

        char* e = expect;
        for(int i = 0; i < 26; i++) {
            mh1 += new PartFileHandle(path1_, i, 1);
            mh1 += new PartFileHandle(path2_, i, 1);

            *e++ = buf1[i];
            *e++ = buf2[i];
        }

        std::cout << mh1 << std::endl;
        std::cout << mh1.estimate() << std::endl;

        mh1.compress();

        std::cout << mh1 << std::endl;
        std::cout << mh1.estimate() << std::endl;

        FileHandle f3(path3_);
        mh1.saveInto(f3);

    }

    DataHandle* fh = path3_.fileHandle();

    fh->openForRead();

    Buffer result(1024);

    EXPECT(fh->read(result, result.size()) == 52);
    fh->close();

    delete fh;

    EXPECT( ::memcmp(expect, result, 52) == 0 );
}



void TestMHHandle::setup()
{
    std::string base = Resource<std::string>("$TMPDIR", "/tmp");
    path1_ = PathName::unique( base + "/path1" );
    path1_ += ".dat";

    path2_ = PathName::unique( base + "/path2" );
    path2_ += ".dat";

    path3_ = PathName::unique( base + "/path3" );
    path3_ += ".dat";
}

void TestMHHandle::teardown()
{
    path1_.unlink();
    path2_.unlink();
    path3_.unlink();
}


CASE("test_multihandle")
{
    TestMHHandle test;
    test.setup();
    test.test_write();
    test.teardown();
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    return run_tests ( argc, argv );
}
