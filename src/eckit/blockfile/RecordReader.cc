// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/blockfile/RecordReader.h"

#include <cstdio>
#include <optional>

#include "zstd.h"

#include "eckit/blockfile/Reader.h"
#include "eckit/exception/Exceptions.h"

namespace eckit::blockfile {

namespace {

constexpr std::size_t plainReadSize = 1 << 20;

bool isZstdMagic(std::uint32_t magic) {
    return magic == zstdFrameMagic || (magic & 0xFFFFFFF0U) == 0x184D2A50U;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

class RecordReader::Impl {
public:

    explicit Impl(const PathName& path) : path_(path.asString()) {
        if (isBlockFile(path)) {
            format_ = Format::BlockFile;
            reader_.emplace(path);
            return;
        }

        file_ = std::fopen(path_.c_str(), "rb");
        if (file_ == nullptr) {
            throw CantOpenFile(path_, false, Here());
        }

        unsigned char magic[4] = {0, 0, 0, 0};
        const std::size_t n    = std::fread(magic, 1, sizeof(magic), file_);
        std::rewind(file_);
        format_ = (n == sizeof(magic) && isZstdMagic(readU32(magic))) ? Format::Zstd : Format::Plain;

        if (format_ == Format::Zstd) {
            stream_ = ZSTD_createDStream();
            if (stream_ == nullptr) {
                release();
                throw FailedLibraryCall("zstd", "ZSTD_createDStream", "allocation failed", Here());
            }
            // Allow archives written with long-distance matching (e.g. `zstd --long`).
            const std::size_t rc = ZSTD_DCtx_setParameter(stream_, ZSTD_d_windowLogMax, 31);
            if (ZSTD_isError(rc)) {
                release();
                throw FailedLibraryCall("zstd", "ZSTD_DCtx_setParameter", ZSTD_getErrorName(rc), Here());
            }
            inputStorage_.resize(ZSTD_DStreamInSize());
            outputStorage_.resize(ZSTD_DStreamOutSize());
        }
        else {
            inputStorage_.resize(plainReadSize);
        }
    }

    ~Impl() { release(); }

    bool next(std::string& record) {
        if (format_ == Format::BlockFile) {
            return nextFromBlocks(record);
        }

        while (true) {
            const std::size_t newline = pending_.find('\n', scanOffset_);
            if (newline != std::string::npos) {
                take(record, newline);
                pendingOffset_ = newline + 1;
                scanOffset_    = pendingOffset_;
                if (!record.empty()) {
                    return true;
                }
                continue;
            }
            scanOffset_ = pending_.size();

            if (finished_) {
                if (pendingOffset_ == pending_.size()) {
                    return false;
                }
                take(record, pending_.size());
                pending_.clear();
                pendingOffset_ = 0;
                scanOffset_    = 0;
                return !record.empty();
            }

            readMore();
        }
    }

    Format format() const { return format_; }

    const std::vector<LabelKey>& keys() const { return reader_ ? reader_->keys() : noKeys_; }

    const Labels& labels() const { return labels_; }

private:

    bool nextFromBlocks(std::string& record) {
        while (true) {
            if (block_ && recordIndex_ < records_.size()) {
                record.assign(records_[recordIndex_].data(), records_[recordIndex_].size());
                ++recordIndex_;
                return true;
            }
            if (nextBlock_ >= reader_->numberOfBlocks()) {
                return false;
            }
            records_.clear();
            block_.reset();
            block_.emplace(reader_->readBlock(nextBlock_++));
            records_     = block_->records();
            recordIndex_ = 0;
            labels_      = block_->info().labels;
        }
    }

    void take(std::string& record, std::size_t end) const {
        record.assign(pending_, pendingOffset_, end - pendingOffset_);
        if (!record.empty() && record.back() == '\r') {
            record.pop_back();
        }
    }

    void readMore() {
        if (pendingOffset_ != 0) {
            pending_.erase(0, pendingOffset_);
            scanOffset_ -= pendingOffset_;
            pendingOffset_ = 0;
        }

        if (format_ == Format::Plain) {
            const std::size_t n = std::fread(inputStorage_.data(), 1, inputStorage_.size(), file_);
            if (n == 0) {
                if (std::ferror(file_) != 0) {
                    throw ReadError(path_, Here());
                }
                finished_ = true;
                return;
            }
            pending_.append(inputStorage_.data(), n);
            return;
        }

        if (input_.pos == input_.size) {
            const std::size_t n = std::fread(inputStorage_.data(), 1, inputStorage_.size(), file_);
            if (n == 0) {
                if (std::ferror(file_) != 0) {
                    throw ReadError(path_, Here());
                }
                if (started_ && remaining_ != 0) {
                    throw BadValue("Truncated zstd file `" + path_ + "`", Here());
                }
                finished_ = true;
                return;
            }
            input_ = ZSTD_inBuffer{inputStorage_.data(), n, 0};
        }

        ZSTD_outBuffer output{outputStorage_.data(), outputStorage_.size(), 0};
        remaining_ = ZSTD_decompressStream(stream_, &output, &input_);
        started_   = true;
        if (ZSTD_isError(remaining_)) {
            throw FailedLibraryCall("zstd", "ZSTD_decompressStream", path_ + ": " + ZSTD_getErrorName(remaining_),
                                    Here());
        }
        pending_.append(outputStorage_.data(), output.pos);
    }

    void release() {
        if (stream_ != nullptr) {
            ZSTD_freeDStream(stream_);
            stream_ = nullptr;
        }
        if (file_ != nullptr) {
            std::fclose(file_);
            file_ = nullptr;
        }
    }

    std::string path_;
    Format format_{Format::Plain};

    // Block files
    std::optional<Reader> reader_;
    std::optional<Block> block_;
    std::vector<std::string_view> records_;
    std::size_t recordIndex_{0};
    std::size_t nextBlock_{0};
    Labels labels_;
    std::vector<LabelKey> noKeys_;

    // Plain and zstd text
    std::FILE* file_{nullptr};
    ZSTD_DStream* stream_{nullptr};
    std::vector<char> inputStorage_;
    std::vector<char> outputStorage_;
    ZSTD_inBuffer input_{nullptr, 0, 0};
    std::string pending_;
    std::size_t pendingOffset_{0};
    std::size_t scanOffset_{0};
    std::size_t remaining_{0};
    bool started_{false};
    bool finished_{false};
};

//----------------------------------------------------------------------------------------------------------------------

RecordReader::RecordReader(const PathName& path) : impl_(std::make_unique<Impl>(path)) {}

RecordReader::~RecordReader() = default;

bool RecordReader::next(std::string& record) {
    return impl_->next(record);
}

RecordReader::Format RecordReader::format() const {
    return impl_->format();
}

const std::vector<LabelKey>& RecordReader::keys() const {
    return impl_->keys();
}

const Labels& RecordReader::labels() const {
    return impl_->labels();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
