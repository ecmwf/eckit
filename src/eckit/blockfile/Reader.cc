// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/blockfile/Reader.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <utility>

#include "zstd.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::blockfile {

//----------------------------------------------------------------------------------------------------------------------

Block::Block(BlockInfo info, Buffer data) : info_(std::move(info)), data_(std::move(data)) {}

std::vector<std::string_view> Block::records() const {
    std::vector<std::string_view> result;
    result.reserve(info_.records);
    forEachRecord([&](std::string_view record) { result.push_back(record); });
    return result;
}

//----------------------------------------------------------------------------------------------------------------------

Reader::Reader(const PathName& path) : path_(path) {
    fd_ = ::open(path_.localPath(), O_RDONLY);
    if (fd_ < 0) {
        throw CantOpenFile(path_.asString(), false, Here());
    }

    try {
        struct stat st;
        if (::fstat(fd_, &st) != 0) {
            throw FailedSystemCall("fstat " + path_.asString(), Here(), errno);
        }
        fileSize_ = static_cast<std::uint64_t>(st.st_size);

        char headerBytes[headerFrameSize];
        if (fileSize_ < headerFrameSize) {
            throw BadValue("Invalid block file `" + path_.asString() + "`: file too small for a header", Here());
        }
        readAt(headerBytes, headerFrameSize, 0);
        header_ = decodeHeaderFrame(headerBytes, headerFrameSize, path_.asString());

        if (!header_.finalised()) {
            throw UserError(
                "Block file `" + path_.asString() + "` was not finalised (the writer did not complete); regenerate it",
                Here());
        }
        if (header_.tableOffset < headerFrameSize || header_.tableOffset + frameHeaderSize > fileSize_) {
            throw BadValue("Invalid block file `" + path_.asString() + "`: block table offset out of range", Here());
        }

        char frameBytes[frameHeaderSize];
        readAt(frameBytes, frameHeaderSize, header_.tableOffset);
        if (readU32(frameBytes) != tableFrameMagic) {
            throw BadValue("Invalid block file `" + path_.asString() + "`: block table not found", Here());
        }
        const std::uint64_t payloadSize = readU32(frameBytes + 4);
        if (header_.tableOffset + frameHeaderSize + payloadSize != fileSize_) {
            throw BadValue("Invalid block file `" + path_.asString() + "`: block table is not the last frame", Here());
        }

        std::string payload(payloadSize, '\0');
        readAt(payload.data(), payload.size(), header_.tableOffset + frameHeaderSize);
        decodeTablePayload(payload.data(), payload.size(), header_.numberOfBlocks, keys_, blocks_, path_.asString());

        checkLayout();
    }
    catch (...) {
        ::close(fd_);
        fd_ = -1;
        throw;
    }
}

Reader::~Reader() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

void Reader::checkLayout() const {
    std::uint64_t records = 0;
    for (const auto& block : blocks_) {
        if (block.offset < headerFrameSize || block.size == 0 || block.offset + block.size > header_.tableOffset) {
            throw BadValue("Invalid block file `" + path_.asString() + "`: block " + std::to_string(block.id) +
                               " lies outside the data region",
                           Here());
        }
        records += block.records;
    }
    if (records != header_.totalRecords) {
        throw BadValue("Invalid block file `" + path_.asString() + "`: header says " +
                           std::to_string(header_.totalRecords) + " records, blocks contain " + std::to_string(records),
                       Here());
    }
}

void Reader::readAt(void* buffer, std::size_t length, std::uint64_t offset) const {
    auto* out = static_cast<char*>(buffer);
    while (length > 0) {
        const ssize_t n = ::pread(fd_, out, length, static_cast<off_t>(offset));
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw ReadError(path_.asString(), Here());
        }
        if (n == 0) {
            throw ShortFile(path_.asString(), Here());
        }
        out += n;
        length -= static_cast<std::size_t>(n);
        offset += static_cast<std::uint64_t>(n);
    }
}

std::vector<std::size_t> Reader::select(const Filter& filter) const {
    std::vector<std::size_t> ids;
    for (const auto& block : blocks_) {
        if (filter.matches(keys_, block)) {
            ids.push_back(block.id);
        }
    }
    return ids;
}

std::string Reader::readCompressed(std::size_t id) const {
    const BlockInfo& block = blocks_.at(id);
    std::string data(block.size, '\0');
    readAt(data.data(), data.size(), block.offset);
    if (readU32(data.data()) != zstdFrameMagic) {
        throw BadValue(
            "Invalid block file `" + path_.asString() + "`: block " + std::to_string(id) + " is not a zstd frame",
            Here());
    }
    return data;
}

Block Reader::readBlock(std::size_t id) const {
    const BlockInfo& block       = blocks_.at(id);
    const std::string compressed = readCompressed(id);

    Buffer data(block.uncompressedSize);
    const std::size_t result = ZSTD_decompress(data.data(), data.size(), compressed.data(), compressed.size());
    if (ZSTD_isError(result)) {
        throw FailedLibraryCall(
            "zstd", "ZSTD_decompress",
            "block " + std::to_string(id) + " of `" + path_.asString() + "`: " + ZSTD_getErrorName(result), Here());
    }
    if (result != block.uncompressedSize) {
        throw BadValue("Invalid block file `" + path_.asString() + "`: block " + std::to_string(id) +
                           " decompressed to " + std::to_string(result) + " bytes, expected " +
                           std::to_string(block.uncompressedSize),
                       Here());
    }

    Block decoded(block, std::move(data));
    const char* begin = decoded.data();
    const auto count  = static_cast<std::uint64_t>(std::count(begin, begin + decoded.size(), '\n'));
    if (count != block.records || (decoded.size() != 0 && begin[decoded.size() - 1] != '\n')) {
        throw BadValue("Invalid block file `" + path_.asString() + "`: block " + std::to_string(id) + " holds " +
                           std::to_string(count) + " records, expected " + std::to_string(block.records),
                       Here());
    }
    return decoded;
}

std::vector<std::string> Reader::verify() const {
    std::vector<std::string> problems;

    // Blocks must tile the data region between the header and the table.
    std::vector<const BlockInfo*> ordered;
    for (const auto& block : blocks_) {
        ordered.push_back(&block);
    }
    std::sort(ordered.begin(), ordered.end(),
              [](const BlockInfo* a, const BlockInfo* b) { return a->offset < b->offset; });
    std::uint64_t expected = headerFrameSize;
    for (const auto* block : ordered) {
        if (block->offset != expected) {
            problems.push_back("block " + std::to_string(block->id) + " starts at " + std::to_string(block->offset) +
                               ", expected " + std::to_string(expected) + " (gap or overlap)");
        }
        expected = block->offset + block->size;
    }
    if (expected != header_.tableOffset) {
        problems.push_back("data region ends at " + std::to_string(expected) + ", block table starts at " +
                           std::to_string(header_.tableOffset));
    }

    for (const auto& block : blocks_) {
        try {
            readBlock(block.id);
        }
        catch (const Exception& e) {
            problems.push_back(e.what());
        }
    }
    return problems;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
