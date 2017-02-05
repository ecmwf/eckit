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

using namespace std;
using namespace eckit;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

class TestMHHandle : public Tool {
public:

    TestMHHandle(int argc, char **argv): Tool(argc, argv) {}

    ~TestMHHandle() {}

    virtual void run();

    void setup();
    void teardown();

    void test_write();

    PathName path1_;
    PathName path2_;
    PathName path3_;

};


void TestMHHandle::test_write()
{
    const char buf1 [] = "abcdefghijklmnopqrstuvwxyz";
    const char buf2 [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char expect[26 * 2];

    {
        FileHandle f1(path1_);
        f1.openForWrite();
        f1.write(buf1, sizeof(buf1));
        f1.close();
    }

     {
        FileHandle f2(path2_);
        f2.openForWrite();
        f2.write(buf2, sizeof(buf2));
        f2.close();
    }

    {
        MultiHandle mh1;

        char* e = expect;
        for(i = 0; i < 26; i++) {
            mh1 += new PartFileHandle(path1_, i, 1);
            mh1 += new PartFileHandle(path2_, i, 1);

            *e++ = buf1[i];
            *e++ = buf2[i];
        }

        std::cout << mh1 << std::endl;

        mh1.compress();

        std::cout << mh1 << std::endl;

        mh1.saveInto(path3_);

    }

    DataHandle* fh = path3_.fileHandle();

    fh->openForRead();

    Buffer result(26 * 2);

    fh->read(result, result.size());
    fh->close();

    delete fh;

    ASSERT( expect == string(result) );
}


void TestMHHandle::test_append()
{
    DataHandle* aioh = new AIOHandle(path_);

    aioh->openForAppend(0);

    const char buf [] = "53d50e63a50fba73f0151028a695a238ff06491c";

    aioh->write(buf, sizeof(buf));

    aioh->close();

    delete aioh;

    DataHandle* fh = path_.fileHandle();

    fh->openForRead();

    fh->seek( sizeof(buf) );

    Buffer buf2(1024);

    fh->read(buf2, buf2.size());
    fh->close();

    delete fh;

    ASSERT( buf == string(buf2) );
}


void TestMHHandle::setup()
{
    std::string base = Resource<std::string>("$TMPDIR", "/tmp");
    path1_ = PathName::unique( base + "/lolo" );
    path1_ += ".dat";

    path2_ = PathName::unique( base + "/lolo" );
    path2_ += ".dat";

    path3_ = PathName::unique( base + "/lolo" );
    path3_ += ".dat";
}

void TestMHHandle::teardown()
{
    path1_.unlink();
    path2_.unlink();
    path3_.unlink();
}


void TestMHHandle::run()
{
    setup();

    test_write();

    teardown();
}

} // namespace test
} // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    eckit::test::TestMHHandle app(argc, argv);
    return app.start();
}

