/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdio>
#include <memory>
#include <string>
#include <thread>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/PooledFile.h"
#include "eckit/log/Bytes.h"


namespace eckit {

class PoolFileEntry;

static thread_local std::map<PathName, std::unique_ptr<PoolFileEntry>> pool_;

struct PoolFileEntryStatus {

    off_t position_;
    bool opened_;

    PoolFileEntryStatus() :
        position_(0), opened_(false) {}
};

class PoolFileEntry {
public:
    std::string name_;
    FILE* file_;
    size_t count_;

    std::unique_ptr<Buffer> buffer_;

    std::map<const PooledFile*, PoolFileEntryStatus> statuses_;

    size_t nbOpens_ = 0;
    size_t nbReads_ = 0;
    size_t nbSeeks_ = 0;

public:
    PoolFileEntry(const std::string& name) :
        name_(name), file_(nullptr), count_(0) {}

    void doClose() {
        if (file_) {
            Log::debug<LibEcKit>() << "Closing from file " << name_ << std::endl;
            if (::fclose(file_) != 0) {
                throw PooledFileError(name_, "Failed to close", Here());
            }
            file_ = nullptr;
            buffer_.reset();
        }
    }

    void add(const PooledFile* file) {
        ASSERT(statuses_.find(file) == statuses_.end());
        statuses_[file] = PoolFileEntryStatus();
    }

    void remove(const PooledFile* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        statuses_.erase(s);

        if (statuses_.size() == 0) {
            doClose();
            pool_.erase(name_);
            // No code after !!!
        }
    }

    void open(const PooledFile* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(!s->second.opened_);

        if (!file_) {
            nbOpens_++;
            file_ = ::fopen(name_.c_str(), "r");
            if (!file_) {
                throw PooledFileError(name_, "Failed to open", Here());
            }

            Log::debug<LibEcKit>() << "PooledFile::openForRead " << name_ << std::endl;

            static size_t bufferSize = Resource<size_t>("FileHandleIOBufferSize;$FILEHANDLE_IO_BUFFERSIZE;-FileHandleIOBufferSize", 0);

            if (bufferSize) {
                Log::debug<LibEcKit>() << "PooledFile using " << Bytes(bufferSize) << std::endl;
                buffer_.reset(new Buffer(bufferSize));
                Buffer& b = *(buffer_.get());
                ::setvbuf(file_, b, _IOFBF, bufferSize);
            }
        }

        s->second.opened_   = true;
        s->second.position_ = 0;
    }

    void close(const PooledFile* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        ASSERT(s->second.opened_);
        s->second.opened_ = false;
    }

    int fileno(const PooledFile* file) const {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);
        return ::fileno(file_);
    }

    long read(const PooledFile* file, void* buffer, long len) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);

        if (::fseeko(file_, s->second.position_, SEEK_SET) < 0) {
            throw PooledFileError(name_, "Failed to seek", Here());
        }

        //      Log::debug<LibEcKit>() < "Reading @ position " << s->second.position_ << " file : " << name_ <<
        //      std::endl;

        size_t length = size_t(len);
        size_t n      = ::fread(buffer, 1, length, file_);

        if (n != length && ::ferror(file_)) {
            throw PooledFileError(name_, "Read error", Here());
        }

        s->second.position_ = ::ftello(file_);

        nbReads_++;

        return n;
    }

    long seek(const PooledFile* file, off_t position) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);

        if (::fseeko(file_, position, SEEK_SET) != 0) {
            std::ostringstream s;
            s << name_ << ": cannot seek to " << position << " (file=" << ::fileno(file_) << ")";
            throw ReadError(s.str());
        }

        s->second.position_ = ::ftello(file_);

        ASSERT(s->second.position_ == position);

        nbSeeks_++;

        return s->second.position_;
    }

    long seekEnd(const PooledFile* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);

        if (::fseeko(file_, 0, SEEK_END) != 0) {
            std::ostringstream s;
            s << name_ << ": cannot seek to end (file=" << ::fileno(file_) << ")";
            throw ReadError(s.str());
        }

        s->second.position_ = ::ftello(file_);

        nbSeeks_++;

        return s->second.position_;
    }
};


PooledFile::PooledFile(const PathName& name) :
    name_(name), entry_(nullptr) {
    auto j = pool_.find(name);
    if (j == pool_.end()) {
        pool_.emplace(std::make_pair(name, std::unique_ptr<PoolFileEntry>(new PoolFileEntry(name))));
        j = pool_.find(name);
    }

    entry_ = (*j).second.get();
    entry_->add(this);
}

PooledFile::~PooledFile() {
    ASSERT(entry_);
    entry_->remove(this);
}

void PooledFile::open() {
    ASSERT(entry_);
    entry_->open(this);
}

void PooledFile::close() {
    ASSERT(entry_);
    entry_->close(this);
}

off_t PooledFile::seek(off_t offset) {
    ASSERT(entry_);
    return entry_->seek(this, offset);
}

off_t PooledFile::seekEnd() {
    ASSERT(entry_);
    return entry_->seekEnd(this);
}

off_t PooledFile::rewind() {
    return seek(0);
}

int PooledFile::fileno() const {
    ASSERT(entry_);
    return entry_->fileno(this);
}

size_t PooledFile::nbOpens() const {
    ASSERT(entry_);
    return entry_->nbOpens_;
}

size_t PooledFile::nbReads() const {
    ASSERT(entry_);
    return entry_->nbReads_;
}

size_t PooledFile::nbSeeks() const {
    ASSERT(entry_);
    return entry_->nbSeeks_;
}

long PooledFile::read(void* buffer, long len) {
    ASSERT(entry_);
    return entry_->read(this, buffer, len);
}

PooledFileError::PooledFileError(const std::string& file, const std::string& msg, const CodeLocation& loc) :
    FileError(msg + " : error on pooled file " + file, loc) {}

}  // namespace eckit
