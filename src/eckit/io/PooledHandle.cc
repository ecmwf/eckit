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
#include <string>
#include <thread>

#include "eckit/config/Resource.h"
#include "eckit/config/LibEcKit.h"
#include "eckit/io/PooledHandle.h"
#include "eckit/io/Buffer.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Bytes.h"


namespace eckit {

class PoolHandleEntry;

static thread_local std::map<PathName, PoolHandleEntry*> pool_;

static int maxPooledHandles() {
    static int maxPooledHandles = eckit::Resource<int>("maxPooledHandles",  512);
    return maxPooledHandles;
}

struct PoolHandleEntryStatus {

    Offset position_;
    bool opened_;

    PoolHandleEntryStatus() :
        position_(0),
        opened_(false) {
    }
};

class PoolHandleEntry {
public:
    PathName path_;
    DataHandle* handle_;
    Length estimate_;

    size_t count_;

    std::map<const PooledHandle*, PoolHandleEntryStatus > statuses_;

    size_t nbOpens_  = 0;
    size_t nbReads_  = 0;
    size_t nbSeeks_  = 0;
    size_t nbCloses_ = 0;

public:

    PoolHandleEntry(const PathName& path):
        path_(path),
        handle_(nullptr),
        count_(0)
    {
    }

    void doClose() {
        if (handle_) {
            Log::info() << "PooledHandle::close(" << path_ << ")" << std::endl;
            handle_->close();
            handle_ = nullptr;
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
            handle_ = path_.fileHandle();
            ASSERT(handle_);
            Log::info() << "PooledHandle::openForRead(" << path_ << ")" << std::endl;
            estimate_ = handle_->openForRead();
        }

        s->second.opened_ = true;
        s->second.position_ = 0;

        return estimate_;
    }

    bool canClose() {
        for(auto i = statuses_.begin(); i != statuses_.end(); ++i) {
            if((*i).second.opened_) {
                return false;
            }
        }
        return true;
    }

    void checkMaxPooledHandles() {
        size_t opened = 0;

        for(auto i = pool_.begin(); i != pool_.end(); ++i) {
            if((*i).second->handle_) {
                opened++;
            }
        }

        if(opened >= maxPooledHandles()) {
            Log::info() << "PooledHandle maximum number of open files reached: " << maxPooledHandles() << std::endl;
            for(auto i = pool_.begin(); i != pool_.end(); ++i) {
                if((*i).second->canClose()) {
                    (*i).second->doClose();
                }
            }
        }

    }

    void close(const PooledHandle* file)  {
        auto s = statuses_.find(file);
        ASSERT(s != statuses_.end());

        ASSERT(s->second.opened_);
        s->second.opened_ = false;

        nbCloses_++;
    }

    long read(const PooledHandle* handle, void *buffer, long len) {
        auto s = statuses_.find(handle);
        ASSERT(s != statuses_.end());
        ASSERT(s->second.opened_);

        ASSERT(handle_->seek(s->second.position_) == s->second.position_);

        size_t length = size_t(len);
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


PooledHandle::PooledHandle(const PathName& path):
    path_(path),
    entry_(nullptr)
{
    auto j = pool_.find(path);
    if (j == pool_.end()) {
        pool_[path] = new PoolHandleEntry(path);
        j = pool_.find(path);
    }

    entry_ = (*j).second;
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

long PooledHandle::read(void *buffer, long len) {
    ASSERT(entry_);
    return entry_->read(this, buffer, len);
}


} // namespace eckit

