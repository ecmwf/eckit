// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "eckit/blockfile/Filter.h"
#include "eckit/blockfile/Format.h"
#include "eckit/blockfile/Reader.h"
#include "eckit/blockfile/RecordReader.h"
#include "eckit/blockfile/Writer.h"
#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/parser/JSONParser.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Tokenizer.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::tools {

using namespace eckit::blockfile;

namespace {

/// Parse "0,3,5-7" into block ids.
std::vector<std::size_t> parseIds(const std::string& expression, std::size_t limit) {
    std::vector<std::size_t> ids;
    std::vector<std::string> terms;
    Tokenizer(",")(expression, terms);
    for (const auto& term : terms) {
        const auto dash         = term.find('-');
        const std::size_t first = std::stoul(term.substr(0, dash));
        const std::size_t last  = dash == std::string::npos ? first : std::stoul(term.substr(dash + 1));
        if (first > last || last >= limit) {
            throw UserError("Block range `" + term + "` is outside the " + std::to_string(limit) + " blocks", Here());
        }
        for (std::size_t id = first; id <= last; ++id) {
            ids.push_back(id);
        }
    }
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
    return ids;
}

/// A label key with the JSON path its value is read from, from "name[:type][=json.path]".
struct KeySpec {
    LabelKey key;
    std::string path;
};

std::vector<KeySpec> parseKeySpecs(const std::string& expression) {
    std::vector<KeySpec> specs;
    std::vector<std::string> terms;
    Tokenizer(",")(expression, terms);
    for (const auto& term : terms) {
        const auto equal       = term.find('=');
        const std::string head = term.substr(0, equal);
        const auto colon       = head.find(':');
        KeySpec spec;
        spec.key.name = head.substr(0, colon);
        spec.key.type = colon == std::string::npos ? LabelType::String : labelTypeFromString(head.substr(colon + 1));
        spec.path     = equal == std::string::npos ? spec.key.name : term.substr(equal + 1);
        if (spec.key.name.empty() || spec.path.empty()) {
            throw UserError("Invalid key `" + term + "`, expected name[:type][=json.path]", Here());
        }
        specs.push_back(spec);
    }
    return specs;
}

/// The value at `path` in a JSON record as a label of `type`. A missing value is "" for string
/// keys and an error for numeric keys.
LabelValue labelFromJson(const LocalConfiguration& record, const std::string& path, LabelType type) {
    if (!record.has(path)) {
        if (type == LabelType::String) {
            return std::string{};
        }
        throw UserError("Record has no value at `" + path + "` for a numeric label", Here());
    }
    std::string text;
    if (record.isString(path)) {
        text = record.getString(path);
    }
    else if (record.isBoolean(path)) {
        text = record.getBool(path) ? "true" : "false";
    }
    else if (record.isIntegral(path)) {
        if (type == LabelType::Integer) {
            return static_cast<std::int64_t>(record.getLong(path));
        }
        if (type == LabelType::Double) {
            return static_cast<double>(record.getLong(path));
        }
        text = std::to_string(record.getLong(path));
    }
    else if (record.isFloatingPoint(path)) {
        if (type == LabelType::Double) {
            return record.getDouble(path);
        }
        std::ostringstream os;
        os << record.getDouble(path);
        text = os.str();
    }
    else {
        throw UserError("JSON value at `" + path + "` is not a scalar and cannot be used as a label", Here());
    }
    return parseLabelValue(type, text);
}

/// Key specs from the `keys` entry of a configuration: either a list of "name[:type][=path]"
/// strings, or a list of {name, type (default string), path (default name)} objects.
std::vector<KeySpec> keySpecsFromConfiguration(const LocalConfiguration& config, const std::string& file) {
    std::vector<KeySpec> specs;
    if (!config.has("keys")) {
        return specs;
    }
    if (config.isStringList("keys")) {
        for (const auto& spec : config.getStringVector("keys")) {
            auto parsed = parseKeySpecs(spec);
            specs.insert(specs.end(), parsed.begin(), parsed.end());
        }
        return specs;
    }
    if (!config.isSubConfigurationList("keys")) {
        throw UserError("`keys` in `" + file + "` must be a list of strings or of {name, type, path}", Here());
    }
    for (const auto& key : config.getSubConfigurations("keys")) {
        if (!key.has("name")) {
            throw UserError("Every entry of `keys` in `" + file + "` needs a `name`", Here());
        }
        KeySpec spec;
        spec.key.name = key.getString("name");
        spec.key.type = labelTypeFromString(key.getString("type", "string"));
        spec.path     = key.getString("path", spec.key.name);
        specs.push_back(spec);
    }
    return specs;
}

/// A configuration built from a parsed JSON value (the Value constructor of LocalConfiguration is protected).
class JsonRecord : public LocalConfiguration {
public:

    explicit JsonRecord(const std::string& json) : LocalConfiguration(JSONParser::decodeString(json)) {}
};

/// One CSV field: quoted when it contains the separator, a quote or a line break.
std::string csvField(const std::string& value, char separator) {
    if (value.find_first_of(std::string{separator} + "\"\r\n") == std::string::npos) {
        return value;
    }
    std::string out = "\"";
    for (const char c : value) {
        out += c == '"' ? "\"\"" : std::string(1, c);
    }
    return out + "\"";
}

double ratio(std::uint64_t uncompressed, std::uint64_t compressed) {
    return compressed == 0 ? 0.0 : static_cast<double>(uncompressed) / static_cast<double>(compressed);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

class EckitBlockfileTool : public eckit::Tool {
public:

    EckitBlockfileTool(int argc, char** argv) : eckit::Tool(argc, argv) {
        using eckit::option::SimpleOption;
        options_.push_back(new SimpleOption<bool>("help", "Print this help"));
        options_.push_back(new SimpleOption<std::string>(
            "filter", "Select blocks by label, e.g. stream=enfo/oper,param=130/167,step=0/to/24"));
        options_.push_back(new SimpleOption<std::string>("blocks", "Select blocks by id, e.g. 0,3,5-7"));
        options_.push_back(new SimpleOption<bool>("summary", "info: totals only, no block table"));
        options_.push_back(new SimpleOption<bool>("no-header", "list: do not print the CSV header line"));
        options_.push_back(new SimpleOption<std::string>("separator", "list: field separator (default ,)"));
        options_.push_back(new SimpleOption<std::string>("output", "select/convert: output block file"));
        options_.push_back(
            new SimpleOption<std::string>("keys",
                                          "convert: label keys as name[:string|int|double][=json.path],... "
                                          "e.g. class=mars.class,stream=mars.stream,param:int=mars.param"));
        options_.push_back(
            new SimpleOption<std::string>("config",
                                          "convert: YAML file with keys, records-per-block, block-bytes and level "
                                          "(command-line options override it)"));
        options_.push_back(new SimpleOption<long>("records-per-block", "convert: records per block (default 1000)"));
        options_.push_back(
            new SimpleOption<long>("block-bytes", "convert: uncompressed bytes per block (default 4 MiB)"));
        options_.push_back(new SimpleOption<long>("level", "convert: zstd compression level (default 9)"));
    }

    int start() {
        try {
            for (int i = 1; i < argc(); ++i) {
                if (argv(i) == "--help" || argv(i) == "-h") {
                    usage(std::cout);
                    return 0;
                }
            }
            std::function<void(const std::string&)> dummy = [](const std::string&) {};
            eckit::option::CmdArgs args(dummy, options_, -1, 1);
            return execute(args);
        }
        catch (const std::exception& e) {
            Log::error() << "** " << e.what() << std::endl;
            return 1;
        }
    }

    void run() final {}  // unused

private:

    using Args = eckit::option::CmdArgs;

    void usage(std::ostream& out) const {
        out << "NAME\n      eckit-blockfile - inspect and manipulate eckit::blockfile files\n\n"
            << "SYNOPSIS\n"
            << "      eckit-blockfile info <file> [--summary]\n"
            << "      eckit-blockfile list <file> [--filter <expr>] [--blocks <ids>] [--no-header] [--separator <c>]\n"
            << "      eckit-blockfile cat <file> [--filter <expr>] [--blocks <ids>]\n"
            << "      eckit-blockfile select <file> --output <file> [--filter <expr>] [--blocks <ids>]\n"
            << "      eckit-blockfile stats <file>\n"
            << "      eckit-blockfile verify <file>...\n"
            << "      eckit-blockfile convert <input>... --output <file> [--config <yaml>] [--keys <spec>]\n"
            << "                      [--records-per-block <n>] [--block-bytes <n>] [--level <n>]\n\n"
            << "DESCRIPTION\n"
            << "      Inputs of convert may be block files, zstd-compressed or plain text files with one record per\n"
            << "      line. Labels come from --keys (JSON paths into each record) or, for block-file inputs, from\n"
            << "      their block labels. Exit status is non-zero on any error or failed verification.\n\n"
            << "      list prints one CSV line per block: block,size,startOffset,endOffset,records followed by one\n"
            << "      column per label key. size is the compressed size in bytes and endOffset = startOffset + size\n"
            << "      (exclusive).\n\n"
            << "      Example --config file:\n"
            << "        keys:\n"
            << "          - {name: class,  path: mars.class}\n"
            << "          - {name: stream, path: mars.stream}\n"
            << "          - {name: param,  path: mars.param, type: int}   # string (default), int, double\n"
            << "        records-per-block: 1000\n"
            << "        block-bytes: 4194304\n"
            << "        level: 9\n"
            << "      or the short form   keys: [class=mars.class, stream=mars.stream, param:int=mars.param]\n\n"
            << "OPTIONS\n";
        for (const auto* option : options_) {
            out << "      " << *option << "\n";
        }
    }

    int execute(const Args& args) {
        const std::string command = args(0);
        std::vector<std::string> files;
        for (std::size_t i = 1; i < args.count(); ++i) {
            files.push_back(args(i));
        }

        if (command == "convert") {
            return convert(args, files);
        }
        if (files.empty()) {
            throw UserError("`" + command + "` needs a file argument", Here());
        }
        if (command == "verify") {
            return verify(files);
        }
        if (files.size() != 1) {
            throw UserError("`" + command + "` takes exactly one file", Here());
        }
        if (command == "info") {
            return info(files[0], args.getBool("summary", false));
        }
        if (command == "list") {
            return list(files[0], args);
        }
        if (command == "cat") {
            return cat(files[0], args);
        }
        if (command == "select") {
            return select(files[0], args);
        }
        if (command == "stats") {
            return stats(files[0]);
        }
        throw UserError("Unknown command `" + command + "` (info, list, cat, select, stats, verify, convert)", Here());
    }

    static std::vector<std::size_t> selectedBlocks(const Reader& reader, const Args& args) {
        std::vector<std::size_t> ids =
            args.has("blocks") ? parseIds(args.getString("blocks"), reader.numberOfBlocks()) : reader.select(Filter{});
        if (args.has("filter")) {
            const Filter filter = Filter::parse(args.getString("filter"));
            std::vector<std::size_t> kept;
            for (std::size_t id : ids) {
                if (filter.matches(reader.keys(), reader.blocks()[id])) {
                    kept.push_back(id);
                }
            }
            ids.swap(kept);
        }
        return ids;
    }

    int info(const std::string& file, bool summary) {
        Reader reader{PathName(file)};
        const auto& header = reader.header();

        std::uint64_t compressed   = 0;
        std::uint64_t uncompressed = 0;
        for (const auto& block : reader.blocks()) {
            compressed += block.size;
            uncompressed += block.uncompressedSize;
        }

        std::cout << "file:          " << file << '\n'
                  << "version:       " << header.version << '\n'
                  << "size:          " << reader.fileSize() << " bytes\n"
                  << "blocks:        " << header.numberOfBlocks << '\n'
                  << "records:       " << header.totalRecords << '\n'
                  << "data:          " << compressed << " bytes compressed, " << uncompressed << " uncompressed (ratio "
                  << std::fixed << std::setprecision(2) << ratio(uncompressed, compressed) << ")\n"
                  << "label keys:   ";
        for (const auto& key : reader.keys()) {
            std::cout << ' ' << key.name << '(' << toString(key.type) << ')';
        }
        std::cout << '\n';

        if (!summary) {
            std::cout << '\n'
                      << std::setw(8) << "block" << std::setw(14) << "offset" << std::setw(12) << "size"
                      << std::setw(12) << "usize" << std::setw(8) << "ratio" << std::setw(10) << "records"
                      << "  labels\n";
            for (const auto& block : reader.blocks()) {
                std::cout << std::setw(8) << block.id << std::setw(14) << block.offset << std::setw(12) << block.size
                          << std::setw(12) << block.uncompressedSize << std::setw(8)
                          << ratio(block.uncompressedSize, block.size) << std::setw(10) << block.records << "  "
                          << formatLabels(reader.keys(), block.labels) << '\n';
            }
        }
        std::cout << std::flush;
        return 0;
    }

    int list(const std::string& file, const Args& args) {
        Reader reader{PathName(file)};
        const std::string separator = args.getString("separator", ",");
        if (separator.size() != 1) {
            throw UserError("--separator must be a single character", Here());
        }
        const char sep = separator[0];

        if (!args.getBool("no-header", false)) {
            std::cout << "block" << sep << "size" << sep << "startOffset" << sep << "endOffset" << sep << "records";
            for (const auto& key : reader.keys()) {
                std::cout << sep << csvField(key.name, sep);
            }
            std::cout << '\n';
        }
        for (std::size_t id : selectedBlocks(reader, args)) {
            const BlockInfo& block = reader.blocks()[id];
            std::cout << block.id << sep << block.size << sep << block.offset << sep << block.offset + block.size << sep
                      << block.records;
            for (const auto& label : block.labels) {
                std::cout << sep << csvField(toString(label), sep);
            }
            std::cout << '\n';
        }
        std::cout << std::flush;
        return 0;
    }

    int cat(const std::string& file, const Args& args) {
        Reader reader{PathName(file)};
        for (std::size_t id : selectedBlocks(reader, args)) {
            const Block block = reader.readBlock(id);
            std::cout.write(block.data(), static_cast<std::streamsize>(block.size()));
        }
        std::cout << std::flush;
        return 0;
    }

    int select(const std::string& file, const Args& args) {
        if (!args.has("output")) {
            throw UserError("select needs --output", Here());
        }
        Reader reader{PathName(file)};
        WriterOptions options;
        options.keys = reader.keys();
        Writer writer(PathName(args.getString("output")), options);
        for (std::size_t id : selectedBlocks(reader, args)) {
            const BlockInfo& info = reader.blocks()[id];
            writer.addCompressedBlock({info.labels, info.records, info.uncompressedSize, reader.readCompressed(id)});
        }
        writer.close();
        Log::info() << "Wrote " << writer.numberOfRecords() << " records in " << writer.numberOfBlocks()
                    << " blocks to " << args.getString("output") << std::endl;
        return 0;
    }

    int stats(const std::string& file) {
        Reader reader{PathName(file)};
        const auto& keys = reader.keys();
        for (std::size_t k = 0; k < keys.size(); ++k) {
            std::map<LabelValue, std::pair<std::uint64_t, std::uint64_t>> counts;  // value -> (blocks, records)
            for (const auto& block : reader.blocks()) {
                auto& count = counts[block.labels[k]];
                ++count.first;
                count.second += block.records;
            }
            std::cout << keys[k].name << " (" << toString(keys[k].type) << "):\n";
            for (const auto& [value, count] : counts) {
                const std::string text = toString(value);
                std::cout << "  " << std::setw(18) << std::left << (text.empty() ? "(empty)" : text) << std::right
                          << std::setw(10) << count.second << " records" << std::setw(8) << count.first << " blocks\n";
            }
        }

        std::map<Labels, std::uint64_t> combinations;
        for (const auto& block : reader.blocks()) {
            combinations[block.labels] += block.records;
        }
        std::cout << "combinations:\n";
        for (const auto& [labels, records] : combinations) {
            std::cout << "  " << std::setw(10) << records << "  " << formatLabels(keys, labels) << '\n';
        }
        std::cout << std::flush;
        return 0;
    }

    int verify(const std::vector<std::string>& files) {
        int status = 0;
        for (const auto& file : files) {
            try {
                Reader reader{PathName(file)};
                const auto problems = reader.verify();
                if (problems.empty()) {
                    std::cout << file << ": OK (" << reader.header().numberOfBlocks << " blocks, "
                              << reader.header().totalRecords << " records)" << std::endl;
                    continue;
                }
                std::cout << file << ": " << problems.size() << " problem(s)" << std::endl;
                for (const auto& problem : problems) {
                    std::cout << "  " << problem << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cout << file << ": " << e.what() << std::endl;
            }
            status = 1;
        }
        return status;
    }

    int convert(const Args& args, const std::vector<std::string>& inputs) {
        if (inputs.empty() || !args.has("output")) {
            throw UserError("convert needs input files and --output", Here());
        }

        // Settings come from --config (YAML) and are overridden by the command-line options.
        LocalConfiguration config;
        std::string configFile;
        if (args.has("config")) {
            configFile = args.getString("config");
            config     = LocalConfiguration{YAMLConfiguration{PathName(configFile)}};
        }

        std::vector<KeySpec> specs = keySpecsFromConfiguration(config, configFile);
        if (args.has("keys")) {
            specs = parseKeySpecs(args.getString("keys"));
        }

        const auto setting = [&](const std::string& name, long fallback) {
            return args.has(name) ? args.getLong(name) : config.getLong(name, fallback);
        };

        WriterOptions options;
        options.recordsPerBlock  = static_cast<std::size_t>(setting("records-per-block", 1000));
        options.bytesPerBlock    = static_cast<std::size_t>(setting("block-bytes", 4 * 1024 * 1024));
        options.compressionLevel = static_cast<int>(setting("level", 9));
        if (options.recordsPerBlock == 0 || options.bytesPerBlock == 0) {
            throw UserError("records-per-block and block-bytes must be positive", Here());
        }

        const auto sameKeys = [](const std::vector<LabelKey>& a, const std::vector<LabelKey>& b) {
            return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](const auto& x, const auto& y) {
                       return x.name == y.name && x.type == y.type;
                   });
        };

        std::optional<Writer> writer;
        std::string record;
        Labels labels;
        for (const auto& input : inputs) {
            RecordReader reader{PathName(input)};
            if (specs.empty() && reader.format() != RecordReader::Format::BlockFile) {
                throw UserError(
                    "Input `" + input + "` is not a block file: give the label keys with --keys or --config", Here());
            }

            if (!writer) {
                if (specs.empty()) {
                    options.keys = reader.keys();
                }
                else {
                    for (const auto& spec : specs) {
                        options.keys.push_back(spec.key);
                    }
                }
                writer.emplace(PathName(args.getString("output")), options);
            }
            else if (specs.empty() && !sameKeys(reader.keys(), options.keys)) {
                throw UserError("Input `" + input + "` has different label keys: use --keys", Here());
            }

            while (reader.next(record)) {
                if (specs.empty()) {
                    writer->add(reader.labels(), record);
                    continue;
                }
                const JsonRecord json{record};
                labels.clear();
                for (const auto& spec : specs) {
                    labels.push_back(labelFromJson(json, spec.path, spec.key.type));
                }
                writer->add(labels, record);
            }
        }

        writer->close();
        Log::info() << "Wrote " << writer->numberOfRecords() << " records in " << writer->numberOfBlocks()
                    << " blocks to " << args.getString("output") << std::endl;
        return 0;
    }

    std::vector<eckit::option::Option*> options_;
};

}  // namespace eckit::tools

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::tools::EckitBlockfileTool{argc, argv}.start();
}
