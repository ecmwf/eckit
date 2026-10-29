// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "eckit/blockfile/Filter.h"
#include "eckit/blockfile/Format.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"

namespace eckit::blockfile {

//----------------------------------------------------------------------------------------------------------------------

/// A decompressed block.
class Block {
public:

    Block(BlockInfo info, Buffer data);

    const BlockInfo& info() const { return info_; }

    const char* data() const { return static_cast<const char*>(data_.data()); }
    std::size_t size() const { return info_.uncompressedSize; }

    /// Call `f(std::string_view record)` for every record, without the trailing newline.
    template <typename F>
    void forEachRecord(F&& f) const {
        const char* begin = data();
        const char* end   = begin + size();
        while (begin < end) {
            const auto* newline = static_cast<const char*>(std::memchr(begin, '\n', end - begin));
            const char* stop    = newline == nullptr ? end : newline;
            f(std::string_view(begin, stop - begin));
            begin = stop + 1;
        }
    }

    std::vector<std::string_view> records() const;

private:

    BlockInfo info_;
    Buffer data_;
};

//----------------------------------------------------------------------------------------------------------------------

/// Random access to a finalised block file.
///
/// Opening reads the header and block table only. `readBlock` and `readCompressed` use
/// positional reads and may be called concurrently from several threads.
class Reader {
public:

    explicit Reader(const PathName& path);
    ~Reader();

    Reader(const Reader&)            = delete;
    Reader& operator=(const Reader&) = delete;

    const PathName& path() const { return path_; }
    const FileHeader& header() const { return header_; }
    const std::vector<LabelKey>& keys() const { return keys_; }
    const std::vector<BlockInfo>& blocks() const { return blocks_; }
    std::size_t numberOfBlocks() const { return blocks_.size(); }
    std::uint64_t fileSize() const { return fileSize_; }

    /// Ids of the blocks matching `filter`, in file order.
    std::vector<std::size_t> select(const Filter& filter) const;

    /// Decompress block `id` and check its size and record count.
    Block readBlock(std::size_t id) const;

    /// The raw zstd frame of block `id` (e.g. to copy it to another file without recompressing).
    std::string readCompressed(std::size_t id) const;

    /// Decompress every block and check the file layout. Returns the problems found (empty if none).
    std::vector<std::string> verify() const;

private:

    void readAt(void* buffer, std::size_t length, std::uint64_t offset) const;
    void checkLayout() const;

    PathName path_;
    int fd_{-1};
    std::uint64_t fileSize_{0};
    FileHeader header_;
    std::vector<LabelKey> keys_;
    std::vector<BlockInfo> blocks_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
