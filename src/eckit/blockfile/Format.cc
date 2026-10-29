// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/blockfile/Format.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

namespace eckit::blockfile {

namespace {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
void putLE(std::string& out, T value) {
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        out.push_back(static_cast<char>((static_cast<std::uint64_t>(value) >> (8 * i)) & 0xFF));
    }
}

template <typename T>
T readLE(const unsigned char* data) {
    T value = 0;
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        value |= static_cast<T>(static_cast<T>(data[i]) << (8 * i));
    }
    return value;
}

std::uint64_t doubleBits(double value) {
    std::uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

double doubleFromBits(std::uint64_t bits) {
    double value = 0;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

[[noreturn]] void corrupt(const std::string& path, const std::string& what) {
    throw BadValue("Invalid block file `" + path + "`: " + what, Here());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace

std::uint32_t readU32(const void* data) {
    return readLE<std::uint32_t>(static_cast<const unsigned char*>(data));
}

std::string toString(LabelType type) {
    switch (type) {
        case LabelType::String:
            return "string";
        case LabelType::Integer:
            return "int";
        case LabelType::Double:
            return "double";
    }
    return "unknown";
}

LabelType labelTypeFromString(const std::string& name) {
    if (name == "string" || name == "str") {
        return LabelType::String;
    }
    if (name == "int" || name == "integer") {
        return LabelType::Integer;
    }
    if (name == "double" || name == "float") {
        return LabelType::Double;
    }
    throw UserError("Unknown label type `" + name + "` (string, int, double)", Here());
}

std::string toString(const LabelValue& value) {
    if (const auto* s = std::get_if<std::string>(&value)) {
        return *s;
    }
    if (const auto* i = std::get_if<std::int64_t>(&value)) {
        return std::to_string(*i);
    }
    std::ostringstream os;
    os.precision(std::numeric_limits<double>::max_digits10);
    os << std::get<double>(value);
    return os.str();
}

LabelValue parseLabelValue(LabelType type, const std::string& text) {
    switch (type) {
        case LabelType::String:
            return text;
        case LabelType::Integer: {
            errno                 = 0;
            char* end             = nullptr;
            const long long value = std::strtoll(text.c_str(), &end, 10);
            if (text.empty() || *end != '\0' || errno == ERANGE) {
                throw UserError("`" + text + "` is not a 64-bit integer", Here());
            }
            return static_cast<std::int64_t>(value);
        }
        case LabelType::Double: {
            errno              = 0;
            char* end          = nullptr;
            const double value = std::strtod(text.c_str(), &end);
            if (text.empty() || *end != '\0' || errno == ERANGE || std::isnan(value)) {
                throw UserError("`" + text + "` is not a number", Here());
            }
            return value;
        }
    }
    throw UserError("Unknown label type", Here());
}

std::string formatLabels(const std::vector<LabelKey>& keys, const Labels& labels) {
    std::string out;
    for (std::size_t i = 0; i < keys.size() && i < labels.size(); ++i) {
        out += (i == 0 ? "" : ",") + keys[i].name + "=" + toString(labels[i]);
    }
    return out;
}

void validateKeys(const std::vector<LabelKey>& keys) {
    std::set<std::string> names;
    for (const auto& key : keys) {
        if (key.name.empty() || key.name.size() > maxKeyNameLength) {
            throw UserError("Block file label key `" + key.name + "` must have 1 to " +
                                std::to_string(maxKeyNameLength) + " characters",
                            Here());
        }
        if (key.type != LabelType::String && key.type != LabelType::Integer && key.type != LabelType::Double) {
            throw UserError("Block file label key `" + key.name + "` has an unknown type", Here());
        }
        if (!names.insert(key.name).second) {
            throw UserError("Duplicate block file label key `" + key.name + "`", Here());
        }
    }
}

void validateLabels(const std::vector<LabelKey>& keys, const Labels& labels) {
    if (labels.size() != keys.size()) {
        throw UserError(
            "Expected " + std::to_string(keys.size()) + " block labels, got " + std::to_string(labels.size()), Here());
    }
    for (std::size_t i = 0; i < keys.size(); ++i) {
        if (labels[i].index() != static_cast<std::size_t>(keys[i].type)) {
            throw UserError("Value `" + toString(labels[i]) + "` of label `" + keys[i].name + "` is not of type " +
                                toString(keys[i].type),
                            Here());
        }
        if (const auto* d = std::get_if<double>(&labels[i]); d != nullptr && std::isnan(*d)) {
            throw UserError("Label `" + keys[i].name + "` must not be NaN", Here());
        }
        if (const auto* s = std::get_if<std::string>(&labels[i]); s != nullptr && s->size() > 0xFFFFFFFFULL) {
            throw UserError("Value of label `" + keys[i].name + "` is too long", Here());
        }
    }
}

std::size_t entrySize(const std::vector<LabelKey>& keys) {
    return baseEntrySize + 8 * keys.size();
}

std::string encodeHeaderFrame(const FileHeader& header) {
    std::string out;
    out.reserve(headerFrameSize);
    putLE<std::uint32_t>(out, headerFrameMagic);
    putLE<std::uint32_t>(out, static_cast<std::uint32_t>(headerPayloadSize));
    out.append(formatTag, sizeof(formatTag));
    putLE<std::uint32_t>(out, header.version);
    putLE<std::uint64_t>(out, header.numberOfBlocks);
    putLE<std::uint64_t>(out, header.tableOffset);
    putLE<std::uint64_t>(out, header.totalRecords);
    out.append(headerFrameSize - out.size(), '\0');
    ASSERT(out.size() == headerFrameSize);
    return out;
}

FileHeader decodeHeaderFrame(const void* data, std::size_t size, const std::string& path) {
    if (size < headerFrameSize) {
        corrupt(path, "file too small for a header");
    }
    const auto* bytes = static_cast<const unsigned char*>(data);
    if (readLE<std::uint32_t>(bytes) != headerFrameMagic || std::memcmp(bytes + 8, formatTag, sizeof(formatTag)) != 0) {
        corrupt(path, "missing block file header");
    }
    if (readLE<std::uint32_t>(bytes + 4) != headerPayloadSize) {
        corrupt(path, "unexpected header size");
    }

    FileHeader header;
    header.version        = readLE<std::uint32_t>(bytes + 12);
    header.numberOfBlocks = readLE<std::uint64_t>(bytes + 16);
    header.tableOffset    = readLE<std::uint64_t>(bytes + 24);
    header.totalRecords   = readLE<std::uint64_t>(bytes + 32);
    if (header.version == 0 || header.version > formatVersion) {
        corrupt(path, "unsupported version " + std::to_string(header.version));
    }
    return header;
}

std::string encodeTableFrame(const std::vector<LabelKey>& keys, const std::vector<BlockInfo>& blocks) {
    validateKeys(keys);

    // String table: each distinct string once, in order of first use.
    std::string strings;
    std::map<std::string, std::uint32_t> stringOffsets;
    const auto stringRef = [&](const std::string& value) {
        auto it = stringOffsets.find(value);
        if (it == stringOffsets.end()) {
            if (strings.size() + value.size() > 0xFFFFFFFFULL) {
                throw UserError("Block file string table exceeds 4 GiB", Here());
            }
            it = stringOffsets.emplace(value, static_cast<std::uint32_t>(strings.size())).first;
            strings += value;
        }
        return (static_cast<std::uint64_t>(value.size()) << 32) | it->second;
    };

    std::string entries;
    entries.reserve(blocks.size() * entrySize(keys));
    for (const auto& block : blocks) {
        validateLabels(keys, block.labels);
        putLE<std::uint64_t>(entries, block.offset);
        putLE<std::uint64_t>(entries, block.size);
        putLE<std::uint64_t>(entries, block.uncompressedSize);
        putLE<std::uint32_t>(entries, block.records);
        for (const auto& label : block.labels) {
            std::uint64_t value = 0;
            if (const auto* s = std::get_if<std::string>(&label)) {
                value = stringRef(*s);
            }
            else if (const auto* i = std::get_if<std::int64_t>(&label)) {
                value = static_cast<std::uint64_t>(*i);
            }
            else {
                value = doubleBits(std::get<double>(label));
            }
            putLE<std::uint64_t>(entries, value);
        }
    }

    std::string payload;
    putLE<std::uint32_t>(payload, static_cast<std::uint32_t>(keys.size()));
    for (const auto& key : keys) {
        putLE<std::uint8_t>(payload, static_cast<std::uint8_t>(key.type));
        putLE<std::uint8_t>(payload, 0);
        putLE<std::uint16_t>(payload, static_cast<std::uint16_t>(key.name.size()));
        payload += key.name;
    }
    putLE<std::uint32_t>(payload, static_cast<std::uint32_t>(entrySize(keys)));
    putLE<std::uint32_t>(payload, 0);
    putLE<std::uint64_t>(payload, strings.size());
    payload += entries;
    payload += strings;

    if (payload.size() > 0xFFFFFFFFULL) {
        throw UserError("Block table too large for a single skippable frame", Here());
    }

    std::string out;
    out.reserve(frameHeaderSize + payload.size());
    putLE<std::uint32_t>(out, tableFrameMagic);
    putLE<std::uint32_t>(out, static_cast<std::uint32_t>(payload.size()));
    out += payload;
    return out;
}

void decodeTablePayload(const void* payload, std::size_t size, std::uint64_t numberOfBlocks,
                        std::vector<LabelKey>& keys, std::vector<BlockInfo>& blocks, const std::string& path) {
    const auto* bytes = static_cast<const unsigned char*>(payload);
    std::size_t pos   = 0;
    const auto need   = [&](std::size_t n, const char* what) {
        if (size - pos < n) {
            corrupt(path, std::string{"block table truncated in "} + what);
        }
    };

    need(4, "the key count");
    const std::size_t nKeys = readLE<std::uint32_t>(bytes);
    pos += 4;

    keys.clear();
    for (std::size_t i = 0; i < nKeys; ++i) {
        need(4, "the key list");
        const auto type          = readLE<std::uint8_t>(bytes + pos);
        const std::size_t length = readLE<std::uint16_t>(bytes + pos + 2);
        pos += 4;
        need(length, "the key names");
        keys.push_back(
            LabelKey{std::string(reinterpret_cast<const char*>(bytes + pos), length), static_cast<LabelType>(type)});
        pos += length;
    }
    try {
        validateKeys(keys);
    }
    catch (const UserError& e) {
        corrupt(path, e.what());
    }

    need(16, "the entry description");
    const std::size_t entry         = readLE<std::uint32_t>(bytes + pos);
    const std::uint64_t stringsSize = readLE<std::uint64_t>(bytes + pos + 8);
    pos += 16;
    if (entry < entrySize(keys)) {
        corrupt(path, "block table entry size smaller than its fields");
    }
    if (numberOfBlocks > (size - pos) / entry) {
        corrupt(path, "block table truncated in the entries");
    }
    const std::size_t entriesBytes = numberOfBlocks * entry;
    if (size - pos - entriesBytes != stringsSize) {
        corrupt(path, "block table size does not match its entries and string table");
    }
    const unsigned char* strings = bytes + pos + entriesBytes;

    blocks.clear();
    blocks.reserve(numberOfBlocks);
    for (std::size_t id = 0; id < numberOfBlocks; ++id) {
        const unsigned char* e = bytes + pos + id * entry;
        BlockInfo block;
        block.id               = id;
        block.offset           = readLE<std::uint64_t>(e);
        block.size             = readLE<std::uint64_t>(e + 8);
        block.uncompressedSize = readLE<std::uint64_t>(e + 16);
        block.records          = readLE<std::uint32_t>(e + 24);
        for (std::size_t k = 0; k < keys.size(); ++k) {
            const std::uint64_t value = readLE<std::uint64_t>(e + baseEntrySize + 8 * k);
            switch (keys[k].type) {
                case LabelType::String: {
                    const std::uint64_t offset = value & 0xFFFFFFFFULL;
                    const std::uint64_t length = value >> 32;
                    if (offset + length > stringsSize) {
                        corrupt(path, "string label outside the string table");
                    }
                    block.labels.emplace_back(
                        std::string(reinterpret_cast<const char*>(strings + offset), static_cast<std::size_t>(length)));
                    break;
                }
                case LabelType::Integer:
                    block.labels.emplace_back(static_cast<std::int64_t>(value));
                    break;
                case LabelType::Double:
                    block.labels.emplace_back(doubleFromBits(value));
                    break;
            }
        }
        blocks.push_back(std::move(block));
    }
}

bool isBlockFile(const PathName& path) {
    std::ifstream in(path.asString(), std::ios::binary);
    char buffer[12];
    if (!in.read(buffer, sizeof(buffer))) {
        return false;
    }
    return readU32(buffer) == headerFrameMagic && std::memcmp(buffer + 8, formatTag, sizeof(formatTag)) == 0;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
