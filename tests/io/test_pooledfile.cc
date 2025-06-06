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
#include "eckit/io/AutoCloser.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/PooledFile.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"
#include "eckit/utils/StringTools.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

class Tester {
public:

    Tester() {

        std::string base = Resource<std::string>("$TMPDIR", "/tmp");
        path1_           = PathName::unique(base + "/path1");
        path1_ += ".dat";

        path2_ = PathName::unique(base + "/path2");
        path2_ += ".dat";

        FileHandle f1(path1_);
        f1.openForWrite(0);
        f1.write(message, ::strlen(message));
        f1.close();

        string upper = StringTools::upper(message);

        FileHandle f2(path2_);
        f2.openForWrite(0);
        f2.write(upper.c_str(), upper.size());
        f2.close();
    }

    ~Tester() {
        bool verbose = false;
        path1_.unlink(verbose);
        path2_.unlink(verbose);
    }

    const char* message = "abcdefghijklmnopqrstuvwxyz";

    PathName path1_;
    PathName path2_;
};

CASE("From one PooledFile") {

    Tester test;

    SECTION("Read from begin") {

        Buffer buffer(64);
        buffer.zero();

        PooledFile f1(test.path1_);
        PooledFile f2(test.path1_);

        EXPECT_NO_THROW(f1.open());
        EXPECT_NO_THROW(f2.open());

        EXPECT(f1.read(buffer, 1) == 1);
        EXPECT(buffer[0] == 'a');

        EXPECT(f2.read(buffer, 1) == 1);
        EXPECT(buffer[0] == 'a');

        EXPECT(f1.read(buffer, 13) == 13);
        EXPECT(std::string(buffer) == std::string("bcdefghijklmn"));

        EXPECT(f2.read(buffer, 25) == 25);
        EXPECT(std::string(buffer) == std::string("bcdefghijklmnopqrstuvwxyz"));

        EXPECT(f1.rewind() == 0);
        EXPECT(f1.read(buffer, buffer.size()) == 26);
        EXPECT(std::string(buffer) == std::string("abcdefghijklmnopqrstuvwxyz"));

        EXPECT_NO_THROW(f1.close());
        EXPECT_NO_THROW(f2.close());

        EXPECT(f1.nbOpens() == 1);
        EXPECT(f1.nbReads() == 5);
        EXPECT(f1.nbSeeks() == 1);
    }

    SECTION("Read interlaced") {

        const size_t N     = 4;
        const size_t M     = ::strlen(test.message);
        const size_t bsize = N * M;

        std::vector<char> result(bsize, '\0');
        std::vector<char> buffer(bsize, '\0');

        std::vector<unique_ptr<PooledFile>> files;
        for (size_t i = 0; i < N; ++i) {
            files.emplace_back(new PooledFile(test.path1_));
        }

        for (auto& file : files) {
            EXPECT_NO_THROW(file->open());
        }

        // read interlaced from N pooled files
        [[maybe_unused]] size_t p = 0;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                char b[2];
                long len = files[j]->read(b, 1);
                EXPECT(len == 1);
                p++;

                buffer[i * N + j] = b[0];
                result[i * N + j] = test.message[i];
            }
        }

        std::cout << std::string(result.data(), result.size()) << std::endl;
        std::cout << std::string(buffer.data(), buffer.size()) << std::endl;

        EXPECT(result == buffer);

        EXPECT(files[0]->nbOpens() == 1);
        EXPECT(files[0]->nbReads() == N * M);

        for (auto& file : files) {
            EXPECT_NO_THROW(file->close());
        }
    }
}

CASE("Error handling") {

    SECTION("Failed to open") {
        const char* randomName = "loiuytgbhytresdfgtr";
        PooledFile f(randomName);
        EXPECT_THROWS_AS(f.open(), PooledFileError);
    }

    SECTION("Failed to close is not an error") {
        Tester test;
        PooledFile* f = new PooledFile(test.path1_);
        EXPECT_NO_THROW(f->open());
        EXPECT_NO_THROW(delete f);
    }
}

CASE("Seeking to location") {

    Tester test;

    SECTION("1") {
        Buffer b(32);
        b.zero();

        PooledFile f(test.path1_);
        auto c = closer(f);

        EXPECT_NO_THROW(f.open());

        EXPECT(f.seekEnd() == 26);

        EXPECT(f.seek(10) == 10);
        EXPECT(f.read(b, 1) == 1);

        EXPECT(b[0] == 'k');

        EXPECT(f.seek(25) == 25);
        EXPECT(f.read(b, 1) == 1);

        EXPECT(b[0] == 'z');

        EXPECT(f.seek(5) == 5);
        EXPECT(f.read(b, 1) == 1);

        EXPECT(b[0] == 'f');

        EXPECT(f.seek(0) == 0);
        EXPECT(f.read(b, 1) == 1);

        EXPECT(b[0] == 'a');

        EXPECT(f.seek(60) == 60);
        EXPECT(f.read(b, 1) == 0);  // read past end of file returns 0

        EXPECT(f.seek(20) == 20);
        EXPECT(f.read(b, 10) == 6);  // read until end of file
    }

    SECTION("2") {
        Buffer b(32);
        b.zero();

        PooledFile f1(test.path1_);
        auto c1 = closer(f1);

        PooledFile f2(test.path2_);
        auto c2 = closer(f2);

        PooledFile f3(test.path1_);
        auto c3 = closer(f3);

        PooledFile f4(test.path2_);
        auto c4 = closer(f4);

        EXPECT_NO_THROW(f1.open());
        EXPECT_NO_THROW(f2.open());
        EXPECT_NO_THROW(f3.open());
        EXPECT_NO_THROW(f4.open());

        EXPECT(f1.seek(10) == 10);
        EXPECT(f1.read(b, 1) == 1);

        EXPECT(b[0] == 'k');

        EXPECT(f2.seek(25) == 25);
        EXPECT(f2.read(b, 1) == 1);

        EXPECT(b[0] == 'Z');

        EXPECT(f3.seek(5) == 5);
        EXPECT(f3.read(b, 1) == 1);

        EXPECT(b[0] == 'f');

        EXPECT(f4.seek(0) == 0);
        EXPECT(f4.read(b, 1) == 1);

        EXPECT(b[0] == 'A');
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
