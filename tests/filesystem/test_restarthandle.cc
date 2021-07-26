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
#include <cstring>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/PartFileHandle.h"

#include "eckit/io/Buffer.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/HandleHolder.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

class Restart : public DataHandle, public HandleHolder {
    Length total_;
    Length nextStop_;

public:
    static size_t increment() { return 77773; }  // a prime larger than 4 KiB

    Restart(DataHandle* h) :
        HandleHolder(h), total_(0) { nextStop_ = increment(); }

    virtual Length openForRead() { NOTIMP; }

    virtual void openForWrite(const Length& len) { return handle().openForWrite(len); }

    virtual void openForAppend(const Length& len) { NOTIMP; }

    virtual long read(void* buffer, long len) { NOTIMP; }

    virtual long write(const void* buffer, long len) {
        if (total_ > nextStop_) {

            nextStop_ += len + increment();

            // 67108879 is first prime after 64*1024*1024 -- the default buffer size in saveInto()
            // this way we test that we roll back to data position before the whole buffer size
            Offset backTo = std::max(total_ - Length(67108879), (long long int)0);

            std::cout << "backTo " << backTo << " nextStop " << nextStop_ << std::endl;

            throw RestartTransfer(backTo);
        }
        total_ += len;
        std::cout << "write @ " << total_ << std::endl;
        return handle().write(buffer, len);
    }

    virtual void close() { handle().close(); }

    virtual void flush() { NOTIMP; }

    virtual void rewind() { NOTIMP; }

    virtual void print(std::ostream& os) const { os << "Restart"; }

    virtual void skip(const Length&) { NOTIMP; }

    virtual Offset seek(const Offset&) { NOTIMP; }

    virtual Length estimate() { NOTIMP; }

    virtual Offset position() { NOTIMP; }

    virtual DataHandle* clone() const { NOTIMP; }

    virtual void restartReadFrom(const Offset& offset) { handle().restartReadFrom(offset); }

    virtual void restartWriteFrom(const Offset& offset) { handle().restartWriteFrom(offset); }
};

//----------------------------------------------------------------------------------------------------------------------

class Tester {
public:
    void setup();
    void teardown();
    void test_write();

    PathName path1_;
    PathName path2_;
    PathName path3_;
};


void Tester::test_write() {
    const char buf1[] = "abcdefghijklmnopqrstuvwxyz";
    const char buf2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const size_t N = 1024 * 1024 * 10;

    // create first file
    {

        Buffer b1(N * 26);
        char* p = b1;
        for (size_t i = 0; i < N; i++) {
            memcpy(p, buf1, 26);
            p += 26;
        }

        FileHandle f1(path1_);
        f1.openForWrite(0);
        f1.write(b1, b1.size());

        f1.close();

        std::cout << path1_ << std::endl;
    }

    // create second file
    {
        Buffer b2(N * 26);
        char* p = b2;
        for (size_t i = 0; i < N; i++) {
            memcpy(p, buf2, 26);
            p += 26;
        }


        FileHandle f2(path2_);
        f2.openForWrite(0);
        f2.write(b2, b2.size());

        f2.close();

        std::cout << path2_ << std::endl;
    }

    std::cout << "-------------------------------------------------------" << std::endl;

    MultiHandle mh1;
    {

        for (int i = 0; i < 26; i++) {

            mh1 += new PartFileHandle(path1_, i * N, N);
            mh1 += new PartFileHandle(path2_, i * N, N);
        }

        std::cout << mh1 << " " << mh1.estimate() << std::endl;

        // mh1.compress();

        //         std::cout << mh1 << " " << mh1.estimate() << std::endl;

        std::cout << Here() << std::endl;

        Restart f3(path3_.fileHandle());
        std::cout << f3 << std::endl;
        std::cout << Here() << std::endl;
        mh1.saveInto(f3);
        std::cout << Here() << std::endl;
    }

    DataHandle* fh = path3_.fileHandle();
    EXPECT(fh->compare(mh1));
    delete fh;
}


void Tester::setup() {
    std::string base = Resource<std::string>("$TMPDIR", "/tmp");
    path1_           = PathName::unique(base + "/path1");
    path1_ += ".dat";

    path2_ = PathName::unique(base + "/path2");
    path2_ += ".dat";

    path3_ = PathName::unique(base + "/path3");
    path3_ += ".dat";
}

void Tester::teardown() {
    if (path1_.exists())
        path1_.unlink();
    if (path2_.exists())
        path2_.unlink();
    if (path3_.exists())
        path3_.unlink();
}


CASE("test_restarthandle") {
    Tester test;
    test.setup();
    try {
        test.test_write();
    }
    catch (...) {
        test.teardown();
        throw;
    }
    test.teardown();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
