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

class Tester {
public:
    Tester() {

        std::string base = Resource<std::string>("$TMPDIR", "/tmp");
        path1_           = PathName::unique(base + "/path1");
        path1_ += ".dat";

        {
            FileHandle f1(path1_);
            f1.openForWrite(0);
            f1.write(buf1, sizeof(buf1)-1);
            f1.close();
            std::cout << "created: " << path1_ << std::endl;
        }
    }

    ~Tester() {
        bool verbose = false;
        path1_.unlink(verbose);
    }

    PathName path1_;
};

CASE("PartFileHandle") {

    format(std::cout, Log::fullFormat);

    Tester test;

    SECTION("PartFileHandle seek/skip/position") {
        OffsetList ol0={};
        LengthList ll0={};
        PartFileHandle ph(test.path1_, ol0, ll0);

        ph.openForRead();
        EXPECT_NO_THROW(ph.seek(0));
        {
            char buff[64];
            eckit::zero(buff);
            long r = ph.read(buff, 10);
            EXPECT(r == 0);
        }
        EXPECT(ph.position() == Offset(0));

        EXPECT_NO_THROW(ph.seek(1));
        {
            char buff[64];
            eckit::zero(buff);
            long r = ph.read(buff, 10);
            EXPECT(r == 0);
        }
        EXPECT(ph.position() == Offset(1));


        EXPECT_NO_THROW(ph.skip(4));
        EXPECT(ph.position() == Offset(5));

    }
    SECTION("PartFileHandle seek/skip/position") {
        OffsetList ol1={0,2,6,13,23};
        LengthList ll1={1,2,4,6,8};
        PartFileHandle ph(test.path1_, ol1, ll1);
        // 0         1         2         3
        // 0123456789012345678901234567890
        // abcdefghijklmnopqrstuvwxyz01234
        // = ==  ====   ======    ========
        // a cd  ghij   nopqrs    xyz01234

        ph.openForRead();
        EXPECT_NO_THROW(ph.seek(21));
        {
            char buff[64];
            eckit::zero(buff);
            long r = ph.read(buff, 10);
            EXPECT(r == 0);
        }


        EXPECT(ph.size() == Length(21));

        EXPECT_NO_THROW(ph.seek(10));
        EXPECT(ph.position() == Offset(10));
        {
            char buff[64];
            eckit::zero(buff);
            long r = ph.read(buff, 13);
            EXPECT(r == 11);
            std::cout << std::string(buff) << std::endl;
            EXPECT(std::string(buff) == "qrsxyz01234");
        }
        EXPECT(ph.position() == Offset(21));
        //move to end of first PartFileHandle
        EXPECT_NO_THROW(ph.skip(-2));
        EXPECT(ph.position() == Offset(19));

        EXPECT_NO_THROW(ph.skip(9));
        EXPECT(ph.position() == Offset(28));
        {
            char buff[64];
            eckit::zero(buff);
            long r = ph.read(buff, 10);
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
