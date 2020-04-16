/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/memory/Zero.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

const char buf1[] = "abcdefghijklmnopqrstuvwxyz";
const char buf2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

class Tester {
public:
    Tester() {

        std::string base = Resource<std::string>("$TMPDIR", "/tmp");
        path1_           = PathName::unique(base + "/path1");
        path1_ += ".dat";

        path2_ = PathName::unique(base + "/path2");
        path2_ += ".dat";

        {
            FileHandle f1(path1_);
            f1.openForWrite(0);
            f1.write(buf1, sizeof(buf1)-1);
            f1.close();
            std::cout << "created: " << path1_ << std::endl;
        }

        {
            FileHandle f2(path2_);
            f2.openForWrite(0);
            f2.write(buf2, sizeof(buf2)-1);
            f2.close();
            std::cout << "created: " << path2_ << std::endl;
        }
    }

    ~Tester() {
        bool verbose = false;
        path1_.unlink(verbose);
        path2_.unlink(verbose);
    }

    PathName path1_;
    PathName path2_;
};

CASE("Multihandle") {

    format(std::cout, Log::fullFormat);

    Tester test;

    SECTION("PartFileHandle compress") {

        char expect[26 * 2];

        MultiHandle mh1;
        {
            char* e = expect;
            for (int i = 0; i < 26; i++) {
                mh1 += new PartFileHandle(test.path1_, i, 1);
                mh1 += new PartFileHandle(test.path2_, i, 1);

                *e++ = buf1[i];
                *e++ = buf2[i];
            }

            // std::cout << mh1 << std::endl;
            EXPECT(mh1.size() == Length(52));

            mh1.compress();

            // std::cout << mh1 << std::endl;
            EXPECT(mh1.size() == Length(52));
        }

        MemoryHandle result(128);

        EXPECT_NO_THROW(mh1.saveInto(result));

        EXPECT(::memcmp(expect, result.data(), sizeof(expect)) == 0);
    }

    SECTION("Multihandle seek in FileHandle") {

        MultiHandle mh;
        mh += new FileHandle(test.path1_);
        mh += new FileHandle(test.path2_);

        mh.openForRead();

        EXPECT(mh.size() == Length(26*2));

        EXPECT_NO_THROW(mh.seek(20));
        {
            char buff[64];
            eckit::zero(buff);
            long r = mh.read(buff, 13);
            EXPECT(r == 13);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "uvwxyzABCDEFG");
        }

        EXPECT_NO_THROW(mh.seek(30));
        {
            char buff[64];
            eckit::zero(buff);
            long r = mh.read(buff, 10);
            EXPECT(r == 10);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "EFGHIJKLMN");
        }

        EXPECT_NO_THROW(mh.seek(0));
        {
            char buff[64];
            eckit::zero(buff);
            long r = mh.read(buff, 7);
            EXPECT(r == 7);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "abcdefg");
        }

        EXPECT_NO_THROW(mh.seek(46));
        {
            char buff[64];
            eckit::zero(buff);
            long r = mh.read(buff, 10);
            EXPECT(r == 6);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "UVWXYZ");
        }

        EXPECT_THROWS_AS(mh.seek(64), eckit::BadValue);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
