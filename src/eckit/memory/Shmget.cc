// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <map>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/memory/Shmget.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static eckit::StaticMutex local_mutex;

static long count_;
static long maxCount_;

static size_t length_;
static size_t maxLength_;

static std::map<int, size_t> sizes_;
static std::map<const void*, int> ids_;

//----------------------------------------------------------------------------------------------------------------------

int Shmget::shmget(key_t key, size_t size, int shmflg) {
    int shmid = ::shmget(key, size, shmflg);
    if (shmid >= 0) {
        AutoLock<StaticMutex> lock(local_mutex);
        sizes_[shmid] = size;
    }
    return shmid;
}

void* Shmget::shmat(int shmid, const void* shmaddr, int shmflg) {
    void* addr = ::shmat(shmid, shmaddr, shmflg);
    if (addr) {
        AutoLock<StaticMutex> lock(local_mutex);
        count_++;
        maxCount_ = std::max(count_, maxCount_);

        length_ += sizes_[shmid];
        maxLength_ = std::max(length_, maxLength_);

        ids_[addr] = shmid;
    }
    return addr;
}

int Shmget::shmdt(const void* shmaddr) {
    int code = ::shmdt(shmaddr);
    if (code == 0) {
        AutoLock<StaticMutex> lock(local_mutex);

        count_--;
        length_ -= sizes_[ids_[shmaddr]];

        sizes_.erase(ids_[shmaddr]);
        ids_.erase(shmaddr);
    }
    return code;
}


void Shmget::info(size_t& count, size_t& size) {
    AutoLock<StaticMutex> lock(local_mutex);
    count = count_;
    size  = length_;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
