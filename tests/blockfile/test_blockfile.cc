// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "eckit/blockfile/Filter.h"
#include "eckit/blockfile/Format.h"
#include "eckit/blockfile/Reader.h"
#include "eckit/blockfile/RecordReader.h"
#include "eckit/blockfile/Writer.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::blockfile;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

std::vector<LabelKey> testKeys() {
    return {{"class", LabelType::String},
            {"stream", LabelType::String},
            {"param", LabelType::Integer},
            {"level", LabelType::Double}};
}

Labels labels(const std::string& stream, std::int64_t param, double level) {
    return {std::string{"od"}, stream, param, level};
}

std::string record(std::size_t i, const std::string& stream) {
    return "{\"i\":" + std::to_string(i) + ",\"stream\":\"" + stream + "\"}";
}

/// 25 records, 4 per block: 20 for (oper, 130, 500.0) and 5 for (enfo, 167, 0.5).
PathName writeTestFile(const std::string& name) {
    PathName path(name);
    WriterOptions options;
    options.keys            = testKeys();
    options.recordsPerBlock = 4;
    Writer writer(path, options);
    for (std::size_t i = 0; i < 25; ++i) {
        const bool enfo = i % 5 == 0;
        writer.add(enfo ? labels("enfo", 167, 0.5) : labels("oper", 130, 500.0), record(i, enfo ? "enfo" : "oper"));
    }
    writer.close();
    return path;
}

std::string slurp(const PathName& path) {
    std::ifstream in(path.asString(), std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void spit(const PathName& path, const std::string& data) {
    std::ofstream out(path.asString(), std::ios::binary | std::ios::trunc);
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("label values parse, print and validate by type") {
    EXPECT(parseLabelValue(LabelType::Integer, "130") == LabelValue{std::int64_t{130}});
    EXPECT(parseLabelValue(LabelType::Double, "0.5") == LabelValue{0.5});
    EXPECT(parseLabelValue(LabelType::String, "0001") == LabelValue{std::string{"0001"}});
    EXPECT_THROWS_AS(parseLabelValue(LabelType::Integer, "13a"), UserError);
    EXPECT_THROWS_AS(parseLabelValue(LabelType::Double, "nan"), UserError);
    EXPECT_EQUAL(toString(LabelValue{std::int64_t{-7}}), std::string("-7"));
    EXPECT(labelTypeFromString("int") == LabelType::Integer);
    EXPECT_THROWS_AS(labelTypeFromString("bool"), UserError);

    EXPECT_NO_THROW(validateLabels(testKeys(), labels("oper", 130, 1.0)));
    EXPECT_THROWS_AS(validateLabels(testKeys(), Labels{std::string{"od"}, std::string{"oper"}, 1.0, 1.0}), UserError);
}

CASE("round trip keeps every record and groups blocks by labels") {
    const PathName path = writeTestFile("blockfile_roundtrip.ekbf");

    Reader reader(path);
    EXPECT(reader.header().finalised());
    EXPECT_EQUAL(reader.header().totalRecords, 25);
    EXPECT_EQUAL(reader.keys().size(), 4);
    EXPECT(reader.keys()[2].type == LabelType::Integer);
    EXPECT(reader.keys()[3].type == LabelType::Double);
    // oper: 20 records -> 5 blocks of 4; enfo: 5 records -> 2 blocks (4 + 1).
    EXPECT_EQUAL(reader.numberOfBlocks(), 7);
    EXPECT(reader.verify().empty());

    std::map<std::string, std::size_t> perStream;
    for (const auto& info : reader.blocks()) {
        const std::string stream         = std::get<std::string>(info.labels[1]);
        const std::int64_t expectedParam = stream == "enfo" ? 167 : 130;
        EXPECT_EQUAL(std::get<std::int64_t>(info.labels[2]), expectedParam);
        const Block block = reader.readBlock(info.id);
        EXPECT_EQUAL(block.records().size(), info.records);
        block.forEachRecord([&](std::string_view r) {
            EXPECT(r.find(stream) != std::string_view::npos);
            ++perStream[stream];
        });
    }
    EXPECT_EQUAL(perStream["oper"], 20);
    EXPECT_EQUAL(perStream["enfo"], 5);

    path.unlink();
}

CASE("filters compare typed values and numeric ranges") {
    const PathName path = writeTestFile("blockfile_filter.ekbf");
    Reader reader(path);

    EXPECT_EQUAL(reader.select(Filter{}).size(), reader.numberOfBlocks());
    EXPECT_EQUAL(reader.select(Filter::parse("stream=enfo")).size(), 2);
    EXPECT_EQUAL(reader.select(Filter::parse("stream=enfo/oper,class=od")).size(), 7);
    EXPECT_EQUAL(reader.select(Filter::parse("param=167")).size(), 2);
    EXPECT_EQUAL(reader.select(Filter::parse("param=100/to/150")).size(), 5);
    EXPECT_EQUAL(reader.select(Filter::parse("param=1/to/2/167")).size(), 2);
    EXPECT_EQUAL(reader.select(Filter::parse("level=0.5")).size(), 2);
    EXPECT_EQUAL(reader.select(Filter::parse("level=0/to/1000")).size(), 7);
    EXPECT_EQUAL(reader.select(Filter::parse("stream=an")).size(), 0);

    EXPECT_THROWS_AS(reader.select(Filter::parse("levtype=pl")), UserError);
    EXPECT_THROWS_AS(reader.select(Filter::parse("param=abc")), UserError);
    EXPECT_THROWS_AS(reader.select(Filter::parse("stream=a/to/b")), UserError);
    EXPECT_THROWS_AS(Filter::parse("stream"), UserError);

    path.unlink();
}

CASE("compressed blocks can be copied without recompression") {
    const PathName source = writeTestFile("blockfile_copy_src.ekbf");
    const PathName target("blockfile_copy_dst.ekbf");
    {
        Reader reader(source);
        WriterOptions options;
        options.keys = reader.keys();
        Writer writer(target, options);
        for (std::size_t id : reader.select(Filter::parse("stream=enfo"))) {
            const BlockInfo& info = reader.blocks()[id];
            writer.addCompressedBlock({info.labels, info.records, info.uncompressedSize, reader.readCompressed(id)});
        }
        writer.close();
    }
    Reader copy(target);
    EXPECT_EQUAL(copy.header().totalRecords, 5);
    EXPECT(copy.verify().empty());

    source.unlink();
    target.unlink();
}

CASE("an empty file has no blocks and is valid") {
    const PathName path("blockfile_empty.ekbf");
    {
        WriterOptions options;
        options.keys = testKeys();
        Writer writer(path, options);
        writer.close();
    }
    Reader reader(path);
    EXPECT_EQUAL(reader.numberOfBlocks(), 0);
    EXPECT_EQUAL(reader.header().totalRecords, 0);
    EXPECT_EQUAL(reader.keys().size(), 4);
    EXPECT(reader.verify().empty());
    path.unlink();
}

CASE("record reader reads block files, zstd streams and plain text") {
    const PathName path = writeTestFile("blockfile_records.ekbf");
    {
        RecordReader reader(path);
        EXPECT(reader.format() == RecordReader::Format::BlockFile);
        std::string r;
        std::size_t n = 0;
        while (reader.next(r)) {
            EXPECT_EQUAL(reader.labels().size(), 4);
            ++n;
        }
        EXPECT_EQUAL(n, 25);
    }

    const PathName plain("blockfile_records.jsonl");
    spit(plain, "a\n\nb\r\nc");
    {
        RecordReader reader(plain);
        EXPECT(reader.format() == RecordReader::Format::Plain);
        std::vector<std::string> records;
        std::string r;
        while (reader.next(r)) {
            records.push_back(r);
        }
        EXPECT_EQUAL(records.size(), 3);
        EXPECT_EQUAL(records[1], std::string("b"));
    }

    path.unlink();
    plain.unlink();
}

CASE("unfinalised, truncated and corrupted files are rejected") {
    const PathName good     = writeTestFile("blockfile_good.ekbf");
    const std::string bytes = slurp(good);

    const PathName unfinalised("blockfile_unfinalised.ekbf");
    {
        WriterOptions options;
        options.keys = testKeys();
        Writer writer(unfinalised, options);
        writer.add(labels("oper", 130, 1.0), "{}");
        writer.flush();
        // destroyed without close()
    }
    EXPECT_THROWS_AS(Reader{unfinalised}, UserError);

    const PathName truncated("blockfile_truncated.ekbf");
    spit(truncated, bytes.substr(0, bytes.size() - 10));
    EXPECT_THROWS(Reader{truncated});

    const PathName corrupted("blockfile_corrupted.ekbf");
    std::string damaged = bytes;
    damaged[headerFrameSize + 20] ^= 0x5A;  // inside the first block
    spit(corrupted, damaged);
    {
        Reader reader(corrupted);
        EXPECT(!reader.verify().empty());
    }

    good.unlink();
    unfinalised.unlink();
    truncated.unlink();
    corrupted.unlink();
}

CASE("invalid keys, labels and records are refused") {
    WriterOptions options;
    options.keys = {{"class", LabelType::String}};
    const PathName path("blockfile_invalid.ekbf");
    Writer writer(path, options);
    EXPECT_THROWS_AS(writer.add(Labels{std::int64_t{1}}, "{}"), UserError);
    EXPECT_THROWS_AS(writer.add(Labels{std::string{"od"}, std::string{"extra"}}, "{}"), UserError);
    EXPECT_THROWS_AS(writer.add(Labels{std::string{"od"}}, "a\nb"), BadValue);
    EXPECT_THROWS_AS(writer.add(Labels{std::string{"od"}}, ""), BadValue);
    writer.close();
    path.unlink();

    WriterOptions duplicate;
    duplicate.keys = {{"class", LabelType::String}, {"class", LabelType::Integer}};
    EXPECT_THROWS_AS(Writer(PathName("blockfile_dup.ekbf"), duplicate), UserError);

    WriterOptions nan;
    nan.keys = {{"level", LabelType::Double}};
    const PathName nanPath("blockfile_nan.ekbf");
    Writer nanWriter(nanPath, nan);
    EXPECT_THROWS_AS(nanWriter.add(Labels{std::numeric_limits<double>::quiet_NaN()}, "{}"), UserError);
    nanWriter.close();
    nanPath.unlink();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
