// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// Checks the reader and the encoders against golden.ekbf, a reference file written by the
/// independent implementation in make_golden_blockfile.py. A failure here means the on-disk
/// format changed: bump formatVersion and regenerate the golden file on purpose.

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "eckit/blockfile/Format.h"
#include "eckit/blockfile/Reader.h"
#include "eckit/blockfile/RecordReader.h"
#include "eckit/filesystem/PathName.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::blockfile;
using namespace eckit::testing;

namespace eckit::test {

namespace {

const PathName golden("golden.ekbf");

std::string slurp(const PathName& path) {
    std::ifstream in(path.asString(), std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("golden file decodes to its documented content") {
    Reader reader(golden);

    EXPECT_EQUAL(reader.header().version, 1);
    EXPECT_EQUAL(reader.header().numberOfBlocks, 2);
    EXPECT_EQUAL(reader.header().totalRecords, 3);
    EXPECT_EQUAL(reader.header().tableOffset, 124);

    const auto& keys = reader.keys();
    EXPECT_EQUAL(keys.size(), 3);
    EXPECT_EQUAL(keys[0].name, std::string("class"));
    EXPECT(keys[0].type == LabelType::String);
    EXPECT_EQUAL(keys[1].name, std::string("param"));
    EXPECT(keys[1].type == LabelType::Integer);
    EXPECT_EQUAL(keys[2].name, std::string("level"));
    EXPECT(keys[2].type == LabelType::Double);

    const auto& blocks = reader.blocks();
    EXPECT(blocks[0].labels == (Labels{std::string{"od"}, std::int64_t{130}, 500.0}));
    EXPECT(blocks[1].labels == (Labels{std::string{"rd"}, std::int64_t{167}, -1.5}));
    EXPECT_EQUAL(blocks[0].offset, headerFrameSize);
    EXPECT_EQUAL(blocks[0].records, 2);
    EXPECT_EQUAL(blocks[1].records, 1);

    const auto first  = reader.readBlock(0);
    const auto second = reader.readBlock(1);
    const auto a      = first.records();
    EXPECT_EQUAL(a.size(), 2);
    EXPECT_EQUAL(std::string(a[0]), std::string("{\"a\":1}"));
    EXPECT_EQUAL(std::string(a[1]), std::string("{\"a\":2}"));
    EXPECT_EQUAL(std::string(second.records().at(0)), std::string("{\"b\":\"x\"}"));

    EXPECT(reader.verify().empty());
}

CASE("encoders reproduce the golden header and block table byte for byte") {
    Reader reader(golden);
    const std::string bytes = slurp(golden);

    EXPECT(encodeHeaderFrame(reader.header()) == bytes.substr(0, headerFrameSize));
    EXPECT(encodeTableFrame(reader.keys(), reader.blocks()) == bytes.substr(reader.header().tableOffset));
}

CASE("golden file streams as plain records") {
    RecordReader reader(golden);
    EXPECT(reader.format() == RecordReader::Format::BlockFile);
    std::vector<std::string> records;
    std::string record;
    while (reader.next(record)) {
        records.push_back(record);
    }
    EXPECT_EQUAL(records.size(), 3);
    EXPECT_EQUAL(records[2], std::string("{\"b\":\"x\"}"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
