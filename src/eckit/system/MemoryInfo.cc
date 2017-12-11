/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#include "eckit/system/MemoryInfo.h"

// #include "eckit/eckit_ecbuild_config.h"
// #include "eckit/parser/StringTools.h"

// #include "eckit/filesystem/LocalPathName.h"
// #include "eckit/exception/Exceptions.h"
// #include "eckit/memory/ScopedPtr.h"
#include "eckit/memory/MemoryPool.h"
#include "eckit/memory/Shmget.h"
#include "eckit/memory/MMap.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/BigNum.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

MemoryInfo::MemoryInfo() {
    ::memset(this, 0, sizeof(*this));

    MemoryPool::info(transientUsed_, transientFree_, MemPool::transientPool);
    MemoryPool::info(permanentUsed_, permanentFree_, MemPool::permanentPool);
    MemoryPool::large(largeUsed_, largeFree_);

    MMap::info(mmap_count_, mmap_size_);
    Shmget::info(shm_count_, shm_size_);

}

static void put(std::ostream& out, const char* title, size_t value, bool& printed, bool bytes = true) {
    if (value) {
        if (printed) {
            out << ", ";
        }
        out << title << ": " ;
        if (bytes) {
            out << eckit::Bytes(value);
        }
        else {
            out << eckit::BigNum(value);
        }
        printed = true;
    }
}

void MemoryInfo::print(std::ostream& out) const {

    bool printed = false;


    // getrusage
    put(out, "resident size", resident_size_, printed);
    put(out, "virtual size", virtual_size_, printed);

    // shaher
    put(out, "mmap count", mmap_count_, printed, false);
    put(out, "mmap size", mmap_size_, printed);

    put(out, "shmem count", shm_count_, printed, false);
    put(out, "shmem size", shm_size_, printed);

    // /proc/pid/smap
    put(out, "mapped shared", mapped_shared_, printed);
    put(out, "mapped read", mapped_read_, printed);
    put(out, "mapped write", mapped_write_, printed);
    put(out, "mapped execute", mapped_execute_, printed);
    put(out, "mapped private", mapped_private_, printed);

    // eckit allocators
    put(out, "large used", largeUsed_, printed);
    put(out, "large free", largeFree_, printed);
    put(out, "transient used", transientUsed_, printed);
    put(out, "transient free", transientFree_, printed);
    put(out, "permanent used", permanentUsed_, printed);
    put(out, "permanent Free", permanentFree_, printed);

        // mallino
    put(out, "arena", arena_, printed);
    // put(out, "ordblks", ordblks_, printed);
    // put(out, "smblks", smblks_, printed);
    // put(out, "hblks", hblks_, printed);
    put(out, "hblkhd", hblkhd_, printed);
    put(out, "usmblks", usmblks_, printed);
    put(out, "fsmblks", fsmblks_, printed);
    put(out, "uordblks", uordblks_, printed);
    put(out, "fordblks", fordblks_, printed);
    put(out, "keepcost", keepcost_, printed);


    if (!printed) {
        out << "no information";
    }

}


static void diff(std::ostream& out, const char* title, size_t value, size_t previous, bool& printed, bool bytes = true) {
    if (value != previous) {
        if (printed) {
            out << ", ";
        }
        out << title << ": ";

        if (bytes) {
            out << eckit::Bytes(value);
        }
        else {
            out << eckit::BigNum(value);
        }

        out << " (";
        size_t diff = 0;
        if (value > previous) {
            out << "+";
            diff = value - previous;
        }
        else {
            out << "-" ;
            diff = previous - value;
        }

        if (bytes) {
            out << eckit::Bytes(diff);
        }
        else {
            out << eckit::BigNum(diff);
        }


        out << ")";
        printed = true;
    }
}


void MemoryInfo::delta(std::ostream& out, const MemoryInfo& other) const {

    bool printed = false;


    // getrusage
    diff(out, "resident size", resident_size_, other.resident_size_, printed);
    diff(out, "virtual size", virtual_size_, other.virtual_size_, printed);

    // ----
    diff(out, "mmap count", mmap_count_, other.mmap_count_, printed, false);
    diff(out, "mmap size", mmap_size_, other.mmap_size_, printed);

    diff(out, "shmem count", shm_count_, other.shm_count_, printed, false);
    diff(out, "shmem size", shm_size_, other.shm_size_, printed);

    // /proc/pid/smap
    diff(out, "mapped shared", mapped_shared_, other.mapped_shared_, printed);
    diff(out, "mapped read", mapped_read_, other.mapped_read_, printed);
    diff(out, "mapped write", mapped_write_, other.mapped_write_, printed);
    diff(out, "mapped execute", mapped_execute_, other.mapped_execute_, printed);
    diff(out, "mapped private", mapped_private_, other.mapped_private_, printed);

    // eckit allocators
    diff(out, "large used", largeUsed_, other.largeUsed_, printed);
    diff(out, "large free", largeFree_, other.largeFree_, printed);
    diff(out, "transient used", transientUsed_, other.transientUsed_, printed);
    diff(out, "transient free", transientFree_, other.transientFree_, printed);
    diff(out, "permanent used", permanentUsed_, other.permanentUsed_, printed);
    diff(out, "permanent free", permanentFree_, other.permanentFree_, printed);

    // mallino
    diff(out, "arena", arena_, other.arena_, printed);
    // diff(out, "ordblks", ordblks_, other.ordblks_, printed);
    // diff(out, "smblks", smblks_, other.smblks_, printed);
    // diff(out, "hblks", hblks_, other.hblks_, printed);
    diff(out, "hblkhd", hblkhd_, other.hblkhd_, printed);
    diff(out, "usmblks", usmblks_, other.usmblks_, printed);
    diff(out, "fsmblks", fsmblks_, other.fsmblks_, printed);
    diff(out, "uordblks", uordblks_, other.uordblks_, printed);
    diff(out, "fordblks", fordblks_, other.fordblks_, printed);
    diff(out, "keepcost", keepcost_, other.keepcost_, printed);

    if (!printed) {
        out << "no change";
    }
}


} // namespace system
} // namespace eckit

