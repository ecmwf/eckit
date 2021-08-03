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

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/PooledHandle.h"
#include "eckit/utils/MD5.h"


namespace eckit {

class PoolHandleEntry;

/// @note in anonymous namespace to solve some compilers link issue (eg. xlc)
namespace {
static thread_local std::map<PathName, std::unique_ptr<PoolHandleEntry>> pool_;
}

static size_t maxPooledHandles() {
    static int maxPooledHandles = eckit::Resource<int>("maxPooledHandles", 16);
    return size_t(maxPooledHandles);
}

struct PoolHandleEntryStatus {

    Offset position_;
    bool opened_;

    PoolHandleEntryStatus() :
        position_(0), opened_(false) {}
};

class PoolHandleEntry {
public:
    PathName path_;
    std::unique_ptr<DataHandle> handle_;
    Length estimate_;

    size_t count_;

    std::map<const PooledHandle*, PoolHandleEntryStatus> statuses_;

    size_t nbOpens_  = 0;
    size_t nbReads_  = 0;
    size_t nbSeeks_  = 0;
    size_t nbCloses_ = 0;

public:
    explicit PoolHandleEntry(const PathName& path) :
        path_(path), handle_(nullptr), count_(0) {}
    ~PoolHandleEntry() { LOG_DEBUG_LIB(LibEcKit) << *this << std::endl; }

    friend std::ostream& operator<<(std::ostream& s, const PoolHandleEntry& e) {
        e.print(s);
        return s;
    }

    void print(std::ostream& s) const {
        s << "PoolHandleEntry[" << path_ << ",opens=" << nbOpens_ << ",reads=" << nbReads_ << ",seeks=" << nbSeeks_
          << ",closes=" << nbCloses_ << "]";
    }

    void doClose() {
        if (handle_) {
            LOG_DEBUG_LIB(LibEcKit) << "PooledHandle::close(" << *handle_ << ")" << std::endl;
            handle_->close();
            handle_.reset();
        }
    }

    void add(const PooledHandle* file) {
        ASSERT(statuses_.find(file) == statuses_.end());
        statuses_[file] = PoolHandleEntryStatus();
    }

    void remove(const PooledHandle* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        statuses_.erase(s);

        if (statuses_.size() == 0) {
            doClose();
            pool_.erase(path_);
            // No code after !!!
        }
    }

    Length open(const PooledHandle* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());
        ASSERT(!s->second.opened_);

        if (!handle_) {

            checkMaxPooledHandles();

            nbOpens_++;
            handle_.reset(path_.fileHandle());
            ASSERT(handle_);
            LOG_DEBUG_LIB(LibEcKit) << "PooledHandle::openForRead(" << *handle_ << ")" << std::endl;
            estimate_ = handle_->openForRead();
        }

        s->second.opened_   = true;
        s->second.position_ = 0;

        return estimate_;
    }

    bool canClose() {
        for (auto i = statuses_.begin(); i != statuses_.end(); ++i) {
            if ((*i).second.opened_) {
                return false;
            }
        }
        return true;
    }

    void checkMaxPooledHandles() {
        size_t opened = 0;

        for (auto i = pool_.begin(); i != pool_.end(); ++i) {
            if ((*i).second->handle_) {
                opened++;
            }
        }

        if (opened >= maxPooledHandles()) {
            LOG_DEBUG_LIB(LibEcKit) << "PooledHandle maximum number of open files reached: " << maxPooledHandles()
                                    << std::endl;
            for (auto i = pool_.begin(); i != pool_.end(); ++i) {
                if ((*i).second->canClose()) {
                    (*i).second->doClose();
                }
            }
        }
    }

    void close(const PooledHandle* file) {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        ASSERT(s->second.opened_);
        s->second.opened_ = false;

        nbCloses_++;
    }

    long read(const PooledHandle* handle, void* buffer, long len) {
        auto s = statuses_.find(handle);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);

        ASSERT(handle_->seek(s->second.position_) == s->second.position_);

        long n = handle_->read(buffer, len);

        s->second.position_ = handle_->position();
        nbReads_++;

        return n;
    }

    long seek(const PooledHandle* handle, Offset position) {
        auto s = statuses_.find(handle);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);

        ASSERT(handle_->seek(position) == position);

        s->second.position_ = handle_->position();

        ASSERT(s->second.position_ == position);

        nbSeeks_++;

        return s->second.position_;
    }
};


PooledHandle::PooledHandle(const PathName& path) :
    path_(path), entry_(nullptr) {
    auto j = pool_.find(path);
    if (j == pool_.end()) {
        pool_.emplace(std::make_pair(path, std::unique_ptr<PoolHandleEntry>(new PoolHandleEntry(path))));
        j = pool_.find(path);
    }

    entry_ = (*j).second.get();
    entry_->add(this);
}

PooledHandle::~PooledHandle() {
    ASSERT(entry_);
    entry_->remove(this);
}

void PooledHandle::print(std::ostream& s) const {
    s << "PooledHandle[" << path_ << "]";
}


Length PooledHandle::openForRead() {
    ASSERT(entry_);
    return entry_->open(this);
}

void PooledHandle::openForWrite(const Length&) {
    NOTIMP;
}

void PooledHandle::openForAppend(const Length&) {
    NOTIMP;
}

long PooledHandle::write(const void*, long) {
    NOTIMP;
}

void PooledHandle::close() {
    ASSERT(entry_);
    entry_->close(this);
}

Offset PooledHandle::seek(const Offset& offset) {
    ASSERT(entry_);
    return entry_->seek(this, offset);
}

size_t PooledHandle::nbOpens() const {
    ASSERT(entry_);
    return entry_->nbOpens_;
}

size_t PooledHandle::nbReads() const {
    ASSERT(entry_);
    return entry_->nbReads_;
}

size_t PooledHandle::nbSeeks() const {
    ASSERT(entry_);
    return entry_->nbSeeks_;
}

long PooledHandle::read(void* buffer, long len) {
    ASSERT(entry_);
    return entry_->read(this, buffer, len);
}

void PooledHandle::hash(MD5& md5) const {
    md5 << "PooledHandle";
    md5 << std::string(entry_->path_);
}

Offset PooledHandle::position() {
    return entry_->handle_->position();
}

}  // namespace eckit
