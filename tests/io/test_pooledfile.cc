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
#include "eckit/io/MultiHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/io/PooledFile.h"
#include "eckit/types/Types.h"

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

        FileHandle f1(path1_);
        f1.openForWrite(0);
        f1.write(message, ::strlen(message));
        f1.close();

//        path2_ = PathName::unique(base + "/path2");
//        path2_ += ".dat";
//        const char buf2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//        FileHandle f2(path2_);
//        f2.openForWrite(0);
//        f2.write(buf2, sizeof(buf2));
//        f2.close();
    }

    ~Tester() {
        path1_.unlink();
//        path2_.unlink();
    }

    const char* message = "abcdefghijklmnopqrstuvwxyz";

    PathName path1_;
//    PathName path2_;
};

CASE("From one PooledFile") {

    SECTION("Read from begin") {

        Tester test;

        Buffer buffer(1024);

        PooledFile f1 (test.path1_);
        PooledFile f2 (test.path1_);

        f1.open();
        f2.open();

        f1.read(buffer, 1);

        EXPECT(buffer[0] == 'a');

        f2.read(buffer, 1);

        EXPECT(buffer[0] == 'a');

        f1.close();
        f2.close();

        EXPECT(f1.nbOpens() == 1);
        EXPECT(f1.nbReads() == 2);
    }

    SECTION("Read interlaced") {

        Tester test;

        const size_t N = 4;
        const size_t M = ::strlen(test.message);
        const size_t bsize = N*M;

        std::vector<char> result(bsize, '\0');
        std::vector<char> buffer(bsize, '\0');

        std::vector<unique_ptr<PooledFile>> files;
        for(int i = 0; i < N ; ++i) {
            files.emplace_back(new PooledFile(test.path1_));
        }

        for(auto& file: files)
            file->open();

        // read interlaced from N pooled files
        size_t p = 0;
        for(size_t i = 0; i < M; ++i) {
            for(int j = 0; j < N ; ++j)  {
                char b[2];
                long len = files[j]->read(b, 1);
                ASSERT(len = 1);
                p++;

                buffer[i*N + j] = b[0];
                result[i*N + j] = test.message[i];
            }
        }

        std::cout << std::string(result.data(), result.size()) << std::endl;
        std::cout << std::string(buffer.data(), buffer.size()) << std::endl;

        EXPECT(result == buffer);

        EXPECT(files[0]->nbOpens() == 1);
        EXPECT(files[0]->nbReads() == N*M);

        for(auto& file: files)
            file->close();
    }

}


// TEST INTERLACED

// TEST THROW ERROR

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
