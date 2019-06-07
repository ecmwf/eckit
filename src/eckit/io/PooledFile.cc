/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdio.h>

#include "eckit/filesystem/PathName.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/PooledFile.h"

namespace eckit {

class PoolFileEntry;

// Use thread SingleTol
std::map<std::string, PoolFileEntry> pool_;

struct PoolFileEntryStatus {
    off_t position_;
    bool opened_;

    PoolFileEntryStatus():
        position_(0),
        opened_(false) {

        }
};

class PoolFileEntry {
    std::string name_;

    FILE* file_;

    std::map<const PooledFile*, PoolFileEntryStatus > statuses_;

public:

    PoolFileEntry(const std: string& name):
        name_(name),
        count_(0),
        file_(0) {
    }

    void doClose() {
        if(file_) {
            if(fclose(file_) != 0) {
                throw error;
            }
            file_ = 0;
        }
    }

    void add(const PooledFile* file) {
        ASSERT(statuses_.find(file) == statuses_.end());
        statuses_[file] = PoolFileEntryStatus();
    }

    void remove(const PooledFile* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(!s->opened_); // To check if we want that semantic

        statuses_.erase(j);

        if(statuses_.size() == 0) {
            doClose();
            pool_.remove(name_);
            // No code after !!!
        }

    }

    void open(const PooledFile* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        ASSERT(!s->opened_);

        if(!file_) {
            file_ = fopen(name_);
        }

        s->opened_ = true;
        s->position_ = 0;

    }

    void close() {

        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        ASSERT(s->opened_);
        s->opened_ = false;

    }

    long read(void *buffer, long len) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(s->opened_);

        if(::fseeko(file_, s->position_, SEEK_SET)<0) {
            throw Errro;
        }
        long n = ::fread(buffer, 1, size, file_);
        s->position_ = ::ftello(file_);

        return n;
    }

    long seek(off_t position, long len) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(s->opened_);

        if(::fseeko(file_, s->position_, SEEK_SET)<0) {
            return -1;
        }

        s->position_ = ::ftello(file_);

        return s->position_;
    }

}


PooledFile::PooledFile(const std::string& name):
    name_(name),
    entry_(0)

{
    auto j = pool_.find(name);
    if(j == pool_.end()) {
        pool_[name] = PoolFileEntry(name);
        j = pool_.find(name);
    }

    entry_ = (*j).second;
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

off_t PooledFile::seek(off_t offset, int whence) {
    ASSERT(entry_);
    return entry_->seek(this, offset, whence);
}

off_t PooledFile::rewind() {
    return seek(0, SEEK_SET);
}

long PooledFile::read(void *buffer, long len) {
    ASSERT(entry_);
    return entry_->read(this, buffer, len);
}

} // namespace eckit

