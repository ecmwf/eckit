// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "eckit/blockfile/Format.h"
#include "eckit/filesystem/PathName.h"

namespace eckit::blockfile {

//----------------------------------------------------------------------------------------------------------------------

struct WriterOptions {
    std::vector<LabelKey> keys;
    /// A block is written when it reaches this many records...
    std::size_t recordsPerBlock{1000};
    /// ...or this many uncompressed bytes, whichever comes first.
    std::size_t bytesPerBlock{4 * 1024 * 1024};
    int compressionLevel{9};
};

/// A block compressed outside the writer, e.g. by a worker thread (see Writer::compressBlock).
struct CompressedBlock {
    Labels labels;
    std::uint32_t records{0};
    std::uint64_t uncompressedSize{0};
    /// One complete zstd frame.
    std::string data;
};

/// Writes a block file.
///
/// Records are grouped by labels: the writer keeps one open block per label combination and
/// writes it when it is full, so record order is kept within a label combination but not
/// across them. Not thread-safe; compress blocks in parallel with `compressBlock` and pass
/// them to `addCompressedBlock` from one thread.
///
/// `close()` writes the block table and patches the header. A writer destroyed without
/// `close()` leaves an unfinalised file, which readers reject.
class Writer {
public:

    Writer(const PathName& path, WriterOptions options);
    ~Writer();

    Writer(const Writer&)            = delete;
    Writer& operator=(const Writer&) = delete;

    /// Add one record (non-empty, without newlines) with one typed label value per key.
    void add(const Labels& labels, std::string_view record);

    void addCompressedBlock(CompressedBlock block);

    /// Write all open blocks, even if not full.
    void flush();

    void close();

    const std::vector<LabelKey>& keys() const { return options_.keys; }
    std::uint64_t numberOfRecords() const { return totalRecords_; }
    std::size_t numberOfBlocks() const { return blocks_.size(); }

    /// Compress `records` (newline-terminated records, `count` of them) into a block.
    static CompressedBlock compressBlock(Labels labels, std::string_view records, std::uint32_t count,
                                         int compressionLevel);

    /// Throw eckit::BadValue if `record` is empty or contains a newline.
    static void validateRecord(std::string_view record);

private:

    struct OpenBlock {
        std::string data;
        std::uint32_t records{0};
    };

    void writeOpenBlock(const Labels& labels, OpenBlock& block);
    void writeAll(const void* data, std::size_t length);

    PathName path_;
    WriterOptions options_;
    int fd_{-1};
    std::uint64_t offset_{0};
    std::uint64_t totalRecords_{0};
    std::map<Labels, OpenBlock> open_;
    std::vector<BlockInfo> blocks_;
    bool closed_{false};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
