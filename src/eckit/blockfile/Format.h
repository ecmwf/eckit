// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file Format.h
/// @brief On-disk layout of eckit::blockfile files.
///
/// A block file stores newline-separated records in independently zstd-compressed blocks.
/// Every block carries one typed value per label key (e.g. class, stream, param), so readers
/// can select blocks without decompressing them. The file is a valid zstd stream: the header
/// and the block table are zstd skippable frames, so `zstdcat file` prints all records.
///
/// @code
/// [header frame : skippable 0x184D2A5A, fixed 64-byte payload, patched on close]
/// [block 0      : zstd frame of newline-terminated records]
/// ...
/// [block N-1    : zstd frame]
/// [table frame  : skippable 0x184D2A5C, last frame of the file]
/// @endcode
///
/// All integers are little-endian; doubles are IEEE-754 binary64 stored as their u64 bit pattern.
///
/// Header payload (64 bytes):
///   char[4] tag "EKBF" | u32 version | u64 numberOfBlocks | u64 tableOffset | u64 totalRecords | 32 reserved
/// `tableOffset` is the file offset of the table frame; 0 means the file was not finalised.
///
/// Table payload:
///   u32 numberOfKeys
///   numberOfKeys x { u8 type | u8 reserved | u16 nameLength | char[nameLength] name }
///   u32 entrySize | u32 reserved | u64 stringTableSize
///   numberOfBlocks x entry, each entrySize bytes:
///     u64 offset | u64 size | u64 uncompressedSize | u32 records | numberOfKeys x u64 value
///   char[stringTableSize] string table
/// A value is an i64 (type 1), the bits of a double (type 2), or for strings (type 0) a
/// u32 offset into the string table followed by a u32 length. `entrySize` lets later versions
/// append fields to the entries; readers skip the bytes they do not know.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace eckit {
class PathName;
}

namespace eckit::blockfile {

//----------------------------------------------------------------------------------------------------------------------

constexpr std::uint32_t headerFrameMagic = 0x184D2A5A;
constexpr std::uint32_t tableFrameMagic  = 0x184D2A5C;
constexpr std::uint32_t zstdFrameMagic   = 0xFD2FB528;

constexpr char formatTag[4]             = {'E', 'K', 'B', 'F'};
constexpr std::uint32_t formatVersion   = 1;
constexpr std::size_t frameHeaderSize   = 8;
constexpr std::size_t headerPayloadSize = 64;
constexpr std::size_t headerFrameSize   = frameHeaderSize + headerPayloadSize;
constexpr std::size_t maxKeyNameLength  = 255;

/// Size of the fixed part of a block-table entry (offset, size, uncompressedSize, records).
constexpr std::size_t baseEntrySize = 8 + 8 + 8 + 4;

/// Type of a label key. The values are stored on disk.
enum class LabelType : std::uint8_t {
    String  = 0,
    Integer = 1,
    Double  = 2,
};

/// A label value; the alternative index equals the LabelType value.
using LabelValue = std::variant<std::string, std::int64_t, double>;
using Labels     = std::vector<LabelValue>;

struct LabelKey {
    std::string name;
    LabelType type{LabelType::String};
};

struct FileHeader {
    std::uint32_t version{formatVersion};
    std::uint64_t numberOfBlocks{0};
    std::uint64_t tableOffset{0};
    std::uint64_t totalRecords{0};

    bool finalised() const { return tableOffset != 0; }
};

struct BlockInfo {
    std::size_t id{0};
    std::uint64_t offset{0};
    std::uint64_t size{0};
    std::uint64_t uncompressedSize{0};
    std::uint32_t records{0};
    /// One value per label key, in key order.
    Labels labels;
};

//----------------------------------------------------------------------------------------------------------------------

std::string toString(LabelType type);

/// Parse `string`, `int` or `double` (throws eckit::UserError).
LabelType labelTypeFromString(const std::string& name);

std::string toString(const LabelValue& value);

/// Parse a textual value as `type` (throws eckit::UserError if it does not fit).
LabelValue parseLabelValue(LabelType type, const std::string& text);

/// `key=value,key=value` for display.
std::string formatLabels(const std::vector<LabelKey>& keys, const Labels& labels);

/// Throw eckit::UserError unless the keys are valid (unique names of 1..255 characters, known types).
void validateKeys(const std::vector<LabelKey>& keys);

/// Throw eckit::UserError unless there is one value of the right type per key (no NaN).
void validateLabels(const std::vector<LabelKey>& keys, const Labels& labels);

std::size_t entrySize(const std::vector<LabelKey>& keys);

/// Encode the complete header frame (headerFrameSize bytes).
std::string encodeHeaderFrame(const FileHeader& header);

/// Decode a header frame of at least headerFrameSize bytes.
FileHeader decodeHeaderFrame(const void* data, std::size_t size, const std::string& path);

/// Encode the complete table frame.
std::string encodeTableFrame(const std::vector<LabelKey>& keys, const std::vector<BlockInfo>& blocks);

/// Decode a table payload (without the 8-byte frame header).
void decodeTablePayload(const void* payload, std::size_t size, std::uint64_t numberOfBlocks,
                        std::vector<LabelKey>& keys, std::vector<BlockInfo>& blocks, const std::string& path);

/// True if `path` starts with a block-file header frame.
bool isBlockFile(const PathName& path);

/// Little-endian u32 at `data`.
std::uint32_t readU32(const void* data);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
