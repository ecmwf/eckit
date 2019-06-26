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

#include "eckit/io/rados/RadosHandle.h"

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

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
    }

    ~Tester() {
        path1_.unlink();
        path2_.unlink();
        path3_.unlink();
    }

    PathName path1_;
    PathName path2_;
    PathName path3_;
};

CASE("Multihandle") {

    setformat(std::cout, Log::fullFormat);

    Tester test;

    const char buf1[] = "abcdefghijklmnopqrstuvwxyz";
    const char buf2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char expect[26 * 2];

    {
        RadosHandle f1(test.path1_);
        f1.openForWrite(0);
        f1.write(buf1, sizeof(buf1));
        f1.close();

        std::cout << test.path1_ << std::endl;
    }

    {
        RadosHandle f2(test.path2_);
        f2.openForWrite(0);
        f2.write(buf2, sizeof(buf2));
        f2.close();

        std::cout << test.path2_ << std::endl;
    }

    MultiHandle mh1;
    {

        char* e = expect;
        for (int i = 0; i < 26; i++) {
            mh1 += new PartFileHandle(test.path1_, i, 1);
            mh1 += new PartFileHandle(test.path2_, i, 1);

            *e++ = buf1[i];
            *e++ = buf2[i];
        }

//        std::cout << mh1 << std::endl;
        EXPECT(mh1.estimate() == Length(52));

        mh1.compress();

//        std::cout << mh1 << std::endl;
        EXPECT(mh1.estimate() == Length(52));
    }

    MemoryHandle result(128);

    EXPECT_NO_THROW( mh1.saveInto(result ));

    EXPECT(::memcmp(expect, result.data(), sizeof(expect)) == 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
