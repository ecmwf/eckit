// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/blockfile/Writer.h"

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <memory>
#include <utility>

#include "zstd.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::blockfile {

namespace {

struct CCtxDeleter {
    void operator()(ZSTD_CCtx* context) const { ZSTD_freeCCtx(context); }
};

void checkZstd(std::size_t code, const char* function) {
    if (ZSTD_isError(code)) {
        throw FailedLibraryCall("zstd", function, ZSTD_getErrorName(code), Here());
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Writer::Writer(const PathName& path, WriterOptions options) : path_(path), options_(std::move(options)) {
    validateKeys(options_.keys);
    if (options_.recordsPerBlock == 0 || options_.bytesPerBlock == 0) {
        throw UserError("Block file records and bytes per block must be positive", Here());
    }

    fd_ = ::open(path_.localPath(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_ < 0) {
        throw CantOpenFile(path_.asString(), false, Here());
    }

    // Placeholder header (not finalised), patched by close().
    const std::string header = encodeHeaderFrame(FileHeader{});
    writeAll(header.data(), header.size());
}

Writer::~Writer() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

void Writer::validateRecord(std::string_view record) {
    if (record.empty()) {
        throw BadValue("Block file records must not be empty", Here());
    }
    if (std::memchr(record.data(), '\n', record.size()) != nullptr) {
        throw BadValue("Block file records must not contain newlines", Here());
    }
}

void Writer::add(const Labels& labels, std::string_view record) {
    ASSERT(!closed_);
    validateRecord(record);

    auto it = open_.find(labels);
    if (it == open_.end()) {
        validateLabels(options_.keys, labels);
        it = open_.emplace(labels, OpenBlock{}).first;
    }

    OpenBlock& block = it->second;
    block.data.append(record.data(), record.size());
    block.data.push_back('\n');
    ++block.records;

    if (block.records >= options_.recordsPerBlock || block.data.size() >= options_.bytesPerBlock) {
        writeOpenBlock(it->first, block);
        open_.erase(it);
    }
}

CompressedBlock Writer::compressBlock(Labels labels, std::string_view records, std::uint32_t count,
                                      int compressionLevel) {
    std::unique_ptr<ZSTD_CCtx, CCtxDeleter> context(ZSTD_createCCtx());
    if (!context) {
        throw FailedLibraryCall("zstd", "ZSTD_createCCtx", "allocation failed", Here());
    }
    checkZstd(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_compressionLevel, compressionLevel),
              "ZSTD_CCtx_setParameter");
    checkZstd(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_checksumFlag, 1), "ZSTD_CCtx_setParameter");

    CompressedBlock block;
    block.labels           = std::move(labels);
    block.records          = count;
    block.uncompressedSize = records.size();
    block.data.resize(ZSTD_compressBound(records.size()));
    const std::size_t size =
        ZSTD_compress2(context.get(), block.data.data(), block.data.size(), records.data(), records.size());
    checkZstd(size, "ZSTD_compress2");
    block.data.resize(size);
    return block;
}

void Writer::writeOpenBlock(const Labels& labels, OpenBlock& block) {
    addCompressedBlock(compressBlock(labels, block.data, block.records, options_.compressionLevel));
}

void Writer::addCompressedBlock(CompressedBlock block) {
    ASSERT(!closed_);
    validateLabels(options_.keys, block.labels);
    if (block.records == 0 || block.data.size() < 4 || readU32(block.data.data()) != zstdFrameMagic) {
        throw BadValue("Compressed block must hold records and be a zstd frame", Here());
    }

    BlockInfo info;
    info.id               = blocks_.size();
    info.offset           = offset_;
    info.size             = block.data.size();
    info.uncompressedSize = block.uncompressedSize;
    info.records          = block.records;
    info.labels           = std::move(block.labels);

    writeAll(block.data.data(), block.data.size());
    totalRecords_ += info.records;
    blocks_.push_back(std::move(info));
}

void Writer::flush() {
    for (auto& [labels, block] : open_) {
        writeOpenBlock(labels, block);
    }
    open_.clear();
}

void Writer::close() {
    if (closed_) {
        return;
    }
    flush();

    FileHeader header;
    header.numberOfBlocks = blocks_.size();
    header.tableOffset    = offset_;
    header.totalRecords   = totalRecords_;

    const std::string table = encodeTableFrame(options_.keys, blocks_);
    writeAll(table.data(), table.size());

    const std::string frame = encodeHeaderFrame(header);
    if (::pwrite(fd_, frame.data(), frame.size(), 0) != static_cast<ssize_t>(frame.size())) {
        throw WriteError(path_.asString(), Here());
    }

    closed_      = true;
    const int fd = fd_;
    fd_          = -1;
    if (::close(fd) != 0) {
        throw CloseError(path_.asString(), Here());
    }
}

void Writer::writeAll(const void* data, std::size_t length) {
    const auto* in = static_cast<const char*>(data);
    while (length > 0) {
        const ssize_t n = ::write(fd_, in, length);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw WriteError(path_.asString(), Here());
        }
        in += n;
        length -= static_cast<std::size_t>(n);
        offset_ += static_cast<std::uint64_t>(n);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
