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
#include "eckit/io/HandleHolder.h"

using namespace std;
using namespace eckit;

namespace eckit {
namespace test {


class Restart : public DataHandle, public HandleHolder {
    Length total_;
    Length next_;

public:

    Restart(DataHandle* h): HandleHolder(h), total_(0) {
        next_ = 4 * 1024 * 1024  + 1;
    }

    virtual Length openForRead() {
        NOTIMP;
    }

    virtual void openForWrite(const Length& len) {
        return handle().openForWrite(len);
    }

    virtual void openForAppend(const Length& len) {
        NOTIMP;
    }

    virtual long read(void* buffer, long len) {
        NOTIMP;
    }

    virtual long write(const void* buffer, long len) {
        if (total_ > next_) {
            next_ += len + 4 * 1024 * 1024 + 1;
            throw RestartTransfer(total_ - Length(12345));
        }
        total_ += len;
        return handle().write(buffer, len);
    }

    virtual void close() {
        handle().close();
    }

    virtual void flush() {
        NOTIMP;
    }

    virtual void rewind() {
        NOTIMP;
    }

    virtual void print(std::ostream& os) const {
        os << "Restart";
    }

    virtual void skip(const Length&) {
        NOTIMP;
    }

    virtual Offset seek(const Offset&) {
        NOTIMP;
    }

    virtual Length estimate() {
        NOTIMP;
    }

    virtual Offset position() {
        NOTIMP;
    }

    virtual DataHandle* clone() const {
        NOTIMP;
    }

    virtual void restartReadFrom(const Offset& offset) {
        handle().restartReadFrom(offset);
    }

    virtual void restartWriteFrom(const Offset& offset) {
        handle().restartWriteFrom(offset);
    }

};

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

    const size_t N = 1024 * 1024 * 64;

    // setformat(std::cout, Log::fullFormat);


    {

        Buffer b1(N * 26);
        char *p = b1;
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

    {
        Buffer b2(N * 26);
        char *p = b2;
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

    std::cout << "-----" << std::endl;

    MultiHandle mh1;

    {

        for (int i = 0; i < 26; i++) {

            mh1 += new PartFileHandle(path1_, i * N, N);
            mh1 += new PartFileHandle(path2_, i * N, N);

        }

        std::cout << mh1 << std::endl;
        std::cout << mh1.estimate() << std::endl;

        // mh1.compress();

        // std::cout << mh1 << std::endl;
        // std::cout << mh1.estimate() << std::endl;

        Restart f3(path3_.fileHandle());
        mh1.saveInto(f3);

    }

    DataHandle* fh = path3_.fileHandle();
    ASSERT(fh->compare(mh1));
    delete fh;

    // fh->openForRead();

    // Buffer result((26 * 2) * N);

    // ASSERT(fh->read(result, result.size()) == (26 * 2) * N);
    // fh->close();

    // delete fh;

    // ASSERT( ::memcmp(expect, result, (26 * 2) * N) == 0 );
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

