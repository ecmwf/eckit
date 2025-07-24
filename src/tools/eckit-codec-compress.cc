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


#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "eckit/codec/codec.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/utils/Compressor.h"

#include "EckitCodecTool.h"

namespace eckit::tools {

//----------------------------------------------------------------------------------------------------------------------

static const std::string DEFAULT_COMPRESSION = "none";

//----------------------------------------------------------------------------------------------------------------------

struct EckitCodecCompress : public EckitCodecTool {
    std::string briefDescription() override { return "Compress eckit::codec files"; }
    std::string usage() override { return name() + " <in> <out> [OPTION]... [--help,-h]"; }
    std::string longDescription() override {
        return briefDescription() +
               "\n"
               "\n       <in>: path to input eckit::codec file"
               "\n       <out>: path to output eckit::codec file";
    }

    EckitCodecCompress(int argc, char** argv) : EckitCodecTool(argc, argv) {
        add_option(new option::SimpleOption<bool>("list", "compression methods list"));
        add_option(new option::SimpleOption<std::string>("compression", "compression method"));
    }

    int execute(const Args& args) override {
        // Handle list
        if (args.has("list")) {
            const auto* sep = "";
            for (const auto& key : CompressorFactory::instance().keys()) {
                Log::info() << sep << key;
                sep = ", ";
            }
            Log::info() << std::endl;
            return success();
        }

        // User sanity checks
        if (args.count() != 2 || args(0) == args(1)) {
            Log::error() << "Expected two unique arguments." << std::endl;
            return failure();
        }

        // Configuration
        LocalConfiguration config;
        config.set("compression", args.getString("compression", DEFAULT_COMPRESSION));

        // Input/output file streams
        PathName file(args(0));
        if (!file.exists()) {
            Log::error() << "File does not exist: " << file << std::endl;
            return failure();
        }

        for (std::uint64_t offset = 0, filesize = static_cast<std::uint64_t>(file.size()); offset < filesize;) {
            // record keys and reader/writer
            codec::RecordPrinter printer{file, offset};
            auto record_keys = printer.record().keys();
            auto record_size = printer.size();

            codec::RecordReader r(file, offset);
            codec::RecordWriter w;

            auto read_write = [&r, &w, &config](const std::string& key, auto T) {
                if (const auto& meta = r.metadata(key); meta.has("shape")) {
                    codec::ArrayShape shape(meta.getUnsignedVector("shape"));

                    std::vector<decltype(T)> value;
                    r.read(key, value).wait();
                    w.set(key, codec::copy(codec::ArrayReference(value.data(), shape)), config);
                    return;
                }

                decltype(T) value;
                r.read(key, value).wait();
                w.set(key, codec::copy(value), config);
            };

            for (const auto& key : record_keys) {
                auto datatype_str = r.metadata(key).getString("datatype");
                auto datatype     = codec::DataType::str_to_kind(datatype_str);

                if (datatype == codec::DataType::KIND_BYTE) {
                    read_write(key, std::byte{});
                }
                else if (datatype == codec::DataType::KIND_INT32) {
                    read_write(key, std::int32_t{});
                }
                else if (datatype == codec::DataType::KIND_INT64) {
                    read_write(key, std::int64_t{});
                }
                else if (datatype == codec::DataType::KIND_REAL32) {
                    read_write(key, float{});
                }
                else if (datatype == codec::DataType::KIND_REAL64) {
                    read_write(key, double{});
                }
                else if (datatype == codec::DataType::KIND_UINT64) {
                    read_write(key, std::uint64_t{});
                }
                else {
                    throw SeriousBug("Unsupported datatype: '" + datatype_str + "'");
                }
            }

            w.write([](const PathName& path) -> codec::Stream& {
                static codec::FileStream out(path, codec::Mode::write);
                return out;
            }(args(1)));

            offset += record_size;
        }

        return success();
    }
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::tools


int main(int argc, char** argv) {
    return eckit::tools::EckitCodecCompress{argc, argv}.start();
}
