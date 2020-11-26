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

const char buf1[] = "abcdefghijklmnopqrstuvwxyz01234";
const char buf2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ56789";

class Tester {
public:
    Tester() {

        std::string base = Resource<std::string>("$TMPDIR", "/tmp");
        path1_           = PathName::unique(base + "/path1");
        path1_ += ".dat";

        path2_ = PathName::unique(base + "/path2");
        path2_ += ".dat";

        path3_ = PathName::unique(base + "/path3");
        path3_ += ".dat";

        {
            FileHandle f1(path1_);
            f1.openForWrite(0);
            f1.write(buf1, sizeof(buf1) - 1);
            f1.close();
            std::cout << "created: " << path1_ << std::endl;
        }

        {
            FileHandle f2(path2_);
            f2.openForWrite(0);
            f2.write(buf2, sizeof(buf2) - 1);
            f2.write(buf2, sizeof(buf2) - 1);
            f2.close();
            std::cout << "created: " << path2_ << std::endl;
        }

        {
            FileHandle f3(path3_);
            f3.openForWrite(0);
            f3.write(buf1, sizeof(buf1) - 1);
            f3.write(buf1, sizeof(buf1) - 1);
            f3.write(buf1, sizeof(buf1) - 1);
            f3.close();
            std::cout << "created: " << path3_ << std::endl;
        }
    }

    ~Tester() {
        bool verbose = false;
        path1_.unlink(verbose);
        path2_.unlink(verbose);
        path3_.unlink(verbose);
    }

    static Buffer makeBuffer() {
        Buffer buffer(128);
        buffer.zero();
        return buffer;
    }

    PathName path1_;
    PathName path2_;
    PathName path3_;
};

//----------------------------------------------------------------------------------------------------------------------

CASE("Multihandle") {

    format(std::cout, Log::fullFormat);

    Tester test;

    OffsetList ol0 = {};
    LengthList ll0 = {};
    OffsetList ol1 = {0, 2, 6, 13, 23};
    LengthList ll1 = {1, 2, 4, 6, 8};

    SECTION("PartFileHandle compress") {

        char expect[] = "aAbcBCdefgDEFGhijklmnoHIJKLMNOpqrstuvwxyz01234PQRSTUVWXYZ56789";

        MultiHandle mh1;
        {
            for (int i = 0; i < 5; i++) {
                mh1 += new PartFileHandle(test.path1_, (1 << i) - 1, 1 << i);
                mh1 += new PartFileHandle(test.path2_, (1 << i) - 1, 1 << i);
            }

            // std::cout << mh1 << std::endl;
            EXPECT(mh1.size() == Length(62));

            mh1.compress();

            // std::cout << mh1 << std::endl;
            EXPECT(mh1.size() == Length(62));
        }

        MemoryHandle result(128);

        EXPECT_NO_THROW(mh1.saveInto(result));

        EXPECT(::memcmp(expect, result.data(), strlen(expect)) == 0);
    }

    SECTION("Seek/skip/position in empty MultiHandle") {

        MultiHandle mh0;

        Length size = mh0.size();
        EXPECT_EQUAL(size, Length(0));

        mh0.openForRead();
        EXPECT_NO_THROW(mh0.seek(0));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh0.read(buff, 10);
            EXPECT(r == 0);
        }

        EXPECT_THROWS_AS(mh0.seek(1), AssertionFailed);  // seeking beoynd EOF asserts
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh0.read(buff, 10);
            EXPECT(r == 0);
        }

        EXPECT(mh0.position() == Offset(0));

        EXPECT_THROWS_AS(mh0.skip(4), AssertionFailed);  // skipping beoynd EOF asserts
        EXPECT(mh0.position() == Offset(0));
    }

    SECTION("Clinical case of empty PartFileHandles making a MultiHandle") {

        MultiHandle mh1;

        mh1 += new PartFileHandle(test.path1_, ol0, ll0);
        mh1 += new MemoryHandle(0);
        mh1 += new PartFileHandle(test.path3_, ol0, ll0);

        mh1.openForRead();
        EXPECT_NO_THROW(mh1.seek(0));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh1.read(buff, 10);
            EXPECT(r == 0);
        }

        EXPECT_THROWS_AS(mh1.seek(1), AssertionFailed);  // seeking beoynd EOF asserts
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh1.read(buff, 10);
            EXPECT(r == 0);
        }

        EXPECT(mh1.position() == Offset(0));
        EXPECT_THROWS_AS(mh1.skip(4), AssertionFailed);  // skipping beoynd EOF asserts
        EXPECT(mh1.position() == Offset(0));
    }

    SECTION("MultiHandle of PartFileHandles") {

        MultiHandle mh2;

        mh2 += new PartFileHandle(test.path1_, ol1, ll1);
        mh2 += new MemoryHandle(0);
        mh2 += new PartFileHandle(test.path3_, ol0, ll0);

        mh2.openForRead();
        EXPECT_NO_THROW(mh2.seek(21));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh2.read(buff, 10);
            EXPECT(r == 0);
        }
    }

    SECTION("MultiHandle of MemoryHandle") {
        MultiHandle mh3;
        mh3 += new MemoryHandle(100);
        mh3 += new MemoryHandle(100);
        EXPECT_THROWS(mh3.seek(0));           // MultiHandle is not open in read mode
        EXPECT_THROWS(mh3.openForWrite(10));  // MultiHandle cannot be opened in write mode

        MultiHandle mh;
        mh += new PartFileHandle(test.path1_, ol1, ll1);
        // 0         1         2         3
        // 0123456789012345678901234567890
        // abcdefghijklmnopqrstuvwxyz01234
        // = ==  ====   ======    ========
        // a cd  ghij   nopqrs    xyz01234

        OffsetList ol2 = {2, 4, 8, 16, 32};
        LengthList ll2 = {1, 2, 4, 8, 16};
        mh += new PartFileHandle(test.path2_, ol2, ll2);
        // 0         1         2         3         4         5         6
        // 01234567890123456789012345678901234567890123456789012345678901
        // ABCDEFGHIJKLMNOPQRSTUVWXYZ56789ABCDEFGHIJKLMNOPQRSTUVWXYZ56789
        //   = ==  ====    ========        ================
        //   C EF  IJKL    QRSTUVWX        BCDEFGHIJKLMNOPQ

        OffsetList ol3 = {2, 4, 8, 16, 32, 64};
        LengthList ll3 = {1, 2, 4, 8, 16, 29};
        mh += new PartFileHandle(test.path3_, ol3, ll3);
        // 0         1         2         3         4         5         6         7         8         9
        // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012
        // abcdefghijklmnopqrstuvwxyz01234abcdefghijklmnopqrstuvwxyz01234abcdefghijklmnopqrstuvwxyz01234
        //   = ==  ====    ========        ================                =============================
        //   c ef  ijkl    qrstuvwx        bcdefghijklmnopq                cdefghijklmnopqrstuvwxyz01234

        //                                                                                                                 1         1
        //             0         1         2         3         4         5         6         7         8         9 0 1
        //             0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901

        // Unused variable expect[]
        // char expect[] =
        //     "acdghijnopqrsxyz01234CEFIJKLQRSTUVWXBCDEFGHIJKLMNOPQcefijklqrstuvwxbcdefghijklmnopqcdefghijklmnopqrstuvwxy"
        //     "z01234";

        mh.openForRead();

        EXPECT(mh.size() == Length(112));

        EXPECT_NO_THROW(mh.seek(10));
        EXPECT(mh.position() == Offset(10));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 13);
            EXPECT(r == 13);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "qrsxyz01234CE");
        }
        EXPECT(mh.position() == Offset(23));
        // move to end of first PartFileHandle
        EXPECT_NO_THROW(mh.skip(-2));
        EXPECT(mh.position() == Offset(21));

        EXPECT_NO_THROW(mh.skip(9));
        EXPECT(mh.position() == Offset(30));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 10);
            EXPECT(r == 10);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "STUVWXBCDE");
        }

        EXPECT_NO_THROW(mh.seek(0));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 7);
            EXPECT(r == 7);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "acdghij");
        }
        EXPECT(mh.position() == Offset(7));

        EXPECT_NO_THROW(mh.seek(106));
        EXPECT(mh.position() == Offset(106));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 10);
            EXPECT(r == 6);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "z01234");
        }
        EXPECT(mh.position() == Offset(112));

        EXPECT_THROWS_AS(mh.seek(120), AssertionFailed);  // seek beyond EOF throws
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 10);
            EXPECT(r == 0);
        }
        EXPECT(mh.position() == Offset(112));

        EXPECT_NO_THROW(mh.skip(-20));  // go back -20 from 112 => 92
        EXPECT(mh.position() == Offset(92));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 30);
            EXPECT(r == 20);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "lmnopqrstuvwxyz01234");
        }
    }

    SECTION("Multihandle seek in FileHandle") {

        MultiHandle mh;
        mh += new FileHandle(test.path1_);
        mh += new FileHandle(test.path2_);

        mh.openForRead();

        EXPECT(mh.size() == Length(31 * 3));

        EXPECT_NO_THROW(mh.seek(20));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 13);
            EXPECT(r == 13);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "uvwxyz01234AB");
        }

        EXPECT_NO_THROW(mh.seek(35));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 10);
            EXPECT(r == 10);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "EFGHIJKLMN");
        }

        EXPECT_NO_THROW(mh.seek(0));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 7);
            EXPECT(r == 7);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "abcdefg");
        }

        EXPECT_NO_THROW(mh.seek(93 - 6));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 10);
            EXPECT(r == 6);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "Z56789");
        }

        EXPECT_THROWS_AS(mh.seek(100), AssertionFailed);  // Seek beyond EOF throws
        EXPECT(mh.position() == Offset(31 * 3));
        {
            Buffer buff = Tester::makeBuffer();
            long r      = mh.read(buff, 10);
            EXPECT(r == 0);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
