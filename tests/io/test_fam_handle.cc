/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   test_fam_handle.cc
/// @author Metin Cakircali
/// @date   Mar 2026

#include "test_fam_common.h"

#include <cstring>
#include <memory>
#include <sstream>
#include <string>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamHandle.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: write and read back data") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 256;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_OBJ");

    const FamObjectName objName(fam::test_endpoint, FamPath{region_name, object_name});

    const std::string test_data = "Hello, FamHandle world!";

    // write via FamHandle
    {
        std::unique_ptr<DataHandle> handle(objName.dataHandle(true));

        handle->openForWrite(object_size);

        EXPECT(handle->estimate() == Length(object_size));

        const auto written = handle->write(test_data.data(), static_cast<long>(test_data.size()));
        EXPECT_EQUAL(written, static_cast<long>(test_data.size()));

        EXPECT(handle->position() == Offset(static_cast<long long>(test_data.size())));

        handle->close();
    }

    // read back via FamHandle
    {
        std::unique_ptr<DataHandle> handle(objName.dataHandle());

        const auto len = handle->openForRead();
        EXPECT(len == Length(object_size));

        Buffer buf(object_size);
        buf.zero();
        const auto bytes_read = handle->read(buf.data(), static_cast<long>(test_data.size()));
        EXPECT_EQUAL(bytes_read, static_cast<long>(test_data.size()));

        EXPECT(std::string(static_cast<const char*>(buf.data()), test_data.size()) == test_data);

        handle->close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: seek and canSeek") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 128;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_SEEK");

    const FamObjectName objName(fam::test_endpoint, FamPath{region_name, object_name});

    const std::string part1 = "AAAA";
    const std::string part2 = "BBBB";

    // write data in two chunks
    {
        std::unique_ptr<DataHandle> handle(objName.dataHandle(true));
        handle->openForWrite(object_size);

        handle->write(part1.data(), static_cast<long>(part1.size()));
        handle->write(part2.data(), static_cast<long>(part2.size()));

        handle->close();
    }

    // seek to read second chunk
    {
        std::unique_ptr<DataHandle> handle(objName.dataHandle());
        handle->openForRead();

        EXPECT(handle->canSeek());

        const auto seeked = handle->seek(Offset(static_cast<long long>(part1.size())));
        EXPECT(seeked == Offset(static_cast<long long>(part1.size())));

        Buffer buf(part2.size());
        buf.zero();
        const auto bytes = handle->read(buf.data(), static_cast<long>(part2.size()));
        EXPECT_EQUAL(bytes, static_cast<long>(part2.size()));
        EXPECT(std::string(static_cast<const char*>(buf.data()), part2.size()) == part2);

        handle->close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: read returns 0 at end of data") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 32;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_EOF");

    const FamObjectName objName(fam::test_endpoint, FamPath{region_name, object_name});

    const std::string data = "short";

    {
        std::unique_ptr<DataHandle> handle(objName.dataHandle(true));
        handle->openForWrite(object_size);
        handle->write(data.data(), static_cast<long>(data.size()));
        handle->close();
    }

    {
        std::unique_ptr<DataHandle> handle(objName.dataHandle());
        handle->openForRead();

        // read entire object
        Buffer buf(object_size);
        buf.zero();
        handle->read(buf.data(), static_cast<long>(object_size));

        // at EOF — read returns 0
        char extra        = 0;
        const auto result = handle->read(&extra, 1);
        EXPECT_EQUAL(result, 0);

        handle->close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: openForWrite on non-existing object allocates") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 64;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_NEW");

    // object does not exist yet
    const FamObjectName obj_name(fam::test_endpoint, FamPath{region_name, object_name});
    EXPECT_NOT(obj_name.exists());

    {
        std::unique_ptr<DataHandle> handle(obj_name.dataHandle(true));
        handle->openForWrite(object_size);

        EXPECT(handle->size() == Length(object_size));

        const std::string data = "fresh";
        handle->write(data.data(), static_cast<long>(data.size()));
        handle->close();
    }

    EXPECT(obj_name.exists());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: openForWrite on existing object with overwrite") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 64;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_OVR");

    const FamObjectName obj_name(fam::test_endpoint, FamPath{region_name, object_name});

    // create object first
    obj_name.allocate(object_size, true);
    EXPECT(obj_name.exists());

    // open for write with overwrite — should succeed with smaller length
    {
        std::unique_ptr<DataHandle> handle(obj_name.dataHandle(true));
        handle->openForWrite(32);

        const std::string data = "overwritten";
        handle->write(data.data(), static_cast<long>(data.size()));
        handle->close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: print") {
    constexpr eckit::fam::size_t region_size = 4096;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_PRINT");

    const FamObjectName obj_name(fam::test_endpoint, FamPath{region_name, object_name});

    // closed mode
    {
        FamHandle handle(obj_name);
        std::ostringstream oss;
        oss << handle;
        const auto str = oss.str();
        EXPECT(str.find("FamHandle") != std::string::npos);
        EXPECT(str.find("closed") != std::string::npos);
    }

    // read mode
    {
        obj_name.allocate(64, true);
        FamHandle handle(obj_name);
        handle.openForRead();
        std::ostringstream oss;
        oss << handle;
        EXPECT(oss.str().find("read") != std::string::npos);
        handle.close();
    }

    // write mode
    {
        FamHandle handle(obj_name, true);
        handle.openForWrite(64);
        std::ostringstream oss;
        oss << handle;
        EXPECT(oss.str().find("write") != std::string::npos);
        handle.close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: partHandle") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 128;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_PART");

    const FamObjectName obj_name(fam::test_endpoint, FamPath{region_name, object_name});

    const std::string data = "0123456789ABCDEF0123456789ABCDEF";

    // write full data
    {
        std::unique_ptr<DataHandle> handle(obj_name.dataHandle(true));
        handle->openForWrite(object_size);
        handle->write(data.data(), static_cast<long>(data.size()));
        handle->close();
    }

    // partHandle creates a FamHandle then the we seek + read
    {
        OffsetList offsets;
        offsets.push_back(Offset(4));
        LengthList lengths;
        lengths.push_back(Length(8));

        std::unique_ptr<DataHandle> handle(obj_name.partHandle(offsets, lengths));
        handle->openForRead();

        // seek to the desired offset manually
        handle->seek(Offset(4));

        Buffer buf(8);
        buf.zero();
        const auto bytes = handle->read(buf.data(), 8);
        EXPECT_EQUAL(bytes, 8);
        EXPECT(std::string(static_cast<const char*>(buf.data()), 8) == "456789AB");

        handle->close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: flush (is a no-op but) does not crash") {
    constexpr eckit::fam::size_t region_size = 4096;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_FLUSH");

    const FamObjectName obj_name(fam::test_endpoint, FamPath{region_name, object_name});

    FamHandle handle(obj_name, true);
    handle.openForWrite(64);

    EXPECT_NO_THROW(handle.flush());

    handle.close();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamHandle: size before and after open") {
    constexpr eckit::fam::size_t region_size = 4096;
    constexpr eckit::fam::size_t object_size = 96;

    auto region      = tester.makeRandomRegion(region_size);
    auto region_name = region.name();
    auto object_name = fam::TestFam::makeRandomText("HANDLE_SIZE");

    const FamObjectName obj_name(fam::test_endpoint, FamPath{region_name, object_name});

    {
        // size before open returns estimate (0 by default)
        FamHandle handle(obj_name);
        EXPECT(handle.estimate() == Length(0));
        EXPECT(handle.size() == Length(0));
    }

    // allocate and write
    obj_name.allocate(object_size, true);

    {
        // size after openForRead returns object size
        FamHandle handle(obj_name);
        const auto len = handle.openForRead();
        EXPECT(len == Length(object_size));
        EXPECT(handle.size() == Length(object_size));
        handle.close();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
