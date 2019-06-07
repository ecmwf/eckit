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

#include "eckit/io/PooledFile.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {



class PoolEntry {
    std::string name_;

    size_t count_;

    FILE* file_;
public:
    PoolEntry(const std: string& name):
        name_(name),
        count_(0),
        file_(0) {
    }

    void add(PooledFile*) {
        count_++;
    }

    void remove(PooledFile*) {
        count_--;
    }

    void open() {
        if(!file_) {
            file_ = fopen(name_, "r");
            if (file_ == 0){
                            throw CantOpenFile(name_, errno == ENOENT);
            }
        }
    }

    void close() {
        if(count_ == 0) {
            fclose(file_);
        }
    }

}

// Use thread singletin
std::map<std::string, PoolEntry> pool_;


PooledFile::PooledFile(const std::string& name):
    name_(name)
{
    if (pool_.find(name) == pool_.end()) {
        pool_[name] = PoolEntry(name);
    }
    pool_[name].add(this);
}

PooledFile::~PooledFile() {
    ASSERT(pool_.find(name) != pool_.end());
    pool_[name].remove(this);
}

void PooledFile::open() {
    ASSERT(pool_.find(name) != pool_.end());
    return pool_[name_].open(this);
}

void PooledFile::close() noexcept(false) {
    ASSERT(pool_.find(name) != pool_.end());
    return pool_[name_].close(this);
}

PooledFile::operator FILE*() {
    ASSERT(pool_.find(name) != pool_.end());
    return pool_[name_].file();
}

} // namespace eckit

