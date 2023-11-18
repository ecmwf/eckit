/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/codec/FileStream.h"
#include "eckit/codec/Session.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/PooledHandle.h"

#include "eckit/testing/Test.h"


namespace eckit::test {

CASE("Stream interoperability with eckit::DataHandle") {
    SECTION("own pointer") {
        codec::Stream s;
        {
            eckit::DataHandle* datahandle = new eckit::FileHandle("test_io_session.data");
            datahandle->openForWrite(0);
            s = codec::Stream{datahandle};
        }
        s.datahandle().close();
    }
    SECTION("shared pointer") {
        codec::Stream s;
        {
            std::shared_ptr<eckit::DataHandle> datahandle = std::make_shared<eckit::FileHandle>("test_io_session.data");
            datahandle->openForWrite(0);
            s = codec::Stream{datahandle};
        }
        s.datahandle().close();
    }
    SECTION("reference") {
        codec::Stream s;
        eckit::FileHandle datahandle("test_io_session.data");
        datahandle.openForWrite(0);
        s = codec::Stream{datahandle};
        s.datahandle().close();
    }
}

CASE("Test seek-for-write works when opening OutputFileStream for append") {
    std::string s1("write    \n");
    std::string s2("append   \n");
    std::string s3("overwrite\n");
    {
        codec::Stream f = codec::OutputFileStream("append-test");
        f.write(s1.c_str(), s1.size());
    }
    {
        codec::Stream f = codec::OutputFileStream("append-test", codec::Mode::append);
        auto offset     = f.position();
        f.write(s2.c_str(), s2.size());

        // Rewind to beginning of append
        f.seek(offset);
        f.write(s3.c_str(), s3.size());
    }
    {
        codec::Stream f      = codec::InputFileStream("append-test");
        std::string expected = s1 + s3;
        std::string read(expected.size(), ' ');
        f.read(const_cast<char*>(read.data()), read.size());
        EXPECT_EQUAL(read, expected);
    }
}


CASE("Opening same file in same scope") {
    // Opening same file within same scope will avoid opening it multiple times, good for perfmance

    // write a file
    {
        codec::OutputFileStream out("test_io_session.data");
        out.write("line1\n", 6);
        out.write("line2\n", 6);
        out.write("line3\n", 6);
    }

    std::string l1(5, ' ');
    std::string l2(5, ' ');
    std::string l3(5, ' ');

    codec::Stream f1 = codec::InputFileStream{"test_io_session.data"};
    f1.seek(0 * 6);
    f1.read(const_cast<char*>(l1.data()), 5);

    codec::Stream f2 = codec::InputFileStream{"test_io_session.data"};
    f2.seek(1 * 6);
    f2.read(const_cast<char*>(l2.data()), 5);

    codec::Stream f3 = codec::InputFileStream{"test_io_session.data"};
    f3.seek(2 * 6);
    f3.read(const_cast<char*>(l3.data()), 5);

    EXPECT_EQUAL(l1, "line1");
    EXPECT_EQUAL(l2, "line2");
    EXPECT_EQUAL(l3, "line3");

    auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f1.datahandle());
    EXPECT(pooled_handle.nbOpens() == 1);
    EXPECT(pooled_handle.nbSeeks() == 3);
    EXPECT(pooled_handle.nbReads() == 3);
}

CASE("Opening same file in parallel scopes") {
    // Files are opened and closed within each scope, bad for performance

    // write a file
    {
        codec::OutputFileStream out("test_io_session.data");
        out.write("line1\n", 6);
        out.write("line2\n", 6);
        out.write("line3\n", 6);
    }

    std::string l1(5, ' ');
    std::string l2(5, ' ');
    std::string l3(5, ' ');

    {
        codec::Stream f1 = codec::InputFileStream{"test_io_session.data"};
        f1.seek(0 * 6);
        f1.read(const_cast<char*>(l1.data()), 5);
        auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f1.datahandle());
        EXPECT(pooled_handle.nbOpens() == 1);
        EXPECT(pooled_handle.nbSeeks() == 1);
        EXPECT(pooled_handle.nbReads() == 1);
    }
    {
        codec::Stream f2 = codec::InputFileStream{"test_io_session.data"};
        f2.seek(1 * 6);
        f2.read(const_cast<char*>(l2.data()), 5);
        auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f2.datahandle());
        EXPECT(pooled_handle.nbOpens() == 1);
        EXPECT(pooled_handle.nbSeeks() == 1);
        EXPECT(pooled_handle.nbReads() == 1);
    }
    {
        codec::Stream f3 = codec::InputFileStream{"test_io_session.data"};
        f3.seek(2 * 6);
        f3.read(const_cast<char*>(l3.data()), 5);
        auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f3.datahandle());
        EXPECT(pooled_handle.nbOpens() == 1);
        EXPECT(pooled_handle.nbSeeks() == 1);
        EXPECT(pooled_handle.nbReads() == 1);
    }
}

CASE("Opening same file in parallel scopes with Session") {
    // Declaring this in an outer scope will keep storage of InputFileStream
    // within nested scopes, so that files will not be opened/closed repeatedly

    codec::Session session;

    // write a file
    {
        codec::OutputFileStream out("test_io_session.data");
        out.write("line1\n", 6);
        out.write("line2\n", 6);
        out.write("line3\n", 6);
    }


    std::string l1(5, ' ');
    std::string l2(5, ' ');
    std::string l3(5, ' ');

    {
        codec::Stream f1 = codec::InputFileStream{"test_io_session.data"};
        f1.seek(0 * 6);
        f1.read(const_cast<char*>(l1.data()), 5);
        auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f1.datahandle());
        EXPECT(pooled_handle.nbOpens() == 1);
        EXPECT(pooled_handle.nbSeeks() == 1);
        EXPECT(pooled_handle.nbReads() == 1);
    }
    {
        codec::Stream f2 = codec::InputFileStream{"test_io_session.data"};
        f2.seek(1 * 6);
        f2.read(const_cast<char*>(l2.data()), 5);
        auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f2.datahandle());
        EXPECT(pooled_handle.nbOpens() == 1);
        EXPECT(pooled_handle.nbSeeks() == 2);
        EXPECT(pooled_handle.nbReads() == 2);
    }
    {
        codec::Stream f3 = codec::InputFileStream{"test_io_session.data"};
        f3.seek(2 * 6);
        f3.read(const_cast<char*>(l3.data()), 5);
        auto& pooled_handle = dynamic_cast<eckit::PooledHandle&>(f3.datahandle());
        EXPECT(pooled_handle.nbOpens() == 1);
        EXPECT(pooled_handle.nbSeeks() == 3);
        EXPECT(pooled_handle.nbReads() == 3);
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
