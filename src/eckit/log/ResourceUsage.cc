/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/log/Seconds.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"

#include "eckit/system/SystemInfo.h"
#include "eckit/memory/MemoryPool.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage():
    name_("unnamed"),
    out_( std::cout )
{
    init();
}

ResourceUsage::ResourceUsage(const std::string& name, std::ostream& o ):
    name_(name),
    out_(o)
{
    init();
}

ResourceUsage::ResourceUsage(const char* name, std::ostream& o ):
    name_(name),
    out_(o)
{
    init();
}

void ResourceUsage::init() {

    using namespace eckit::system;
    const SystemInfo& sysinfo = SystemInfo::instance();

    Mem usage = sysinfo.memoryUsage();

    rss_ = usage.resident_size_;
    malloc_ = sysinfo.memoryAllocated();
    shared_ = usage.shared_memory_;

    MemoryPool::info(transientUsed_, transientFree_, MemPool::transientPool);
    MemoryPool::info(permanentUsed_, permanentFree_, MemPool::permanentPool);
    MemoryPool::large(largeUsed_, largeFree_);

    out_ << name_
         << " => resident size: "
         << eckit::Bytes(rss_)
         << ", allocated: "
         << eckit::Bytes(malloc_);

    if (shared_) {
        out_ << ", shared: "
             << eckit::Bytes(shared_);
    }

    if (largeUsed_ || largeFree_) {
        out_ << ", large [used: "
             << eckit::Bytes(largeUsed_)
             << ", free: "
             << eckit::Bytes(largeFree_)
             << "]";
    }
    if (transientUsed_ || transientFree_) {

        out_  << ", transient [used: "
              << eckit::Bytes(transientUsed_)
              << ", free: "
              << eckit::Bytes(transientFree_)
              << "]";
    }
    if (permanentUsed_ || permanentFree_) {

        out_ << ", [permanent used: "
             << eckit::Bytes(permanentUsed_)
             << ", free: "
             << eckit::Bytes(permanentFree_)
             << "]";
    }
    out_ << std::endl;
}

ResourceUsage::~ResourceUsage()
{
    using namespace eckit::system;
    const SystemInfo& sysinfo = SystemInfo::instance();

    Mem usage = sysinfo.memoryUsage();

    size_t rss = usage.resident_size_;
    size_t malloc = sysinfo.memoryAllocated();
    size_t shared = usage.shared_memory_;

    size_t largeUsed;

    size_t transientUsed;
    size_t permanentUsed;

    size_t largeFree;

    size_t transientFree;
    size_t permanentFree;


    MemoryPool::info(transientUsed, transientFree, MemPool::transientPool);
    MemoryPool::info(permanentUsed, permanentFree, MemPool::permanentPool);
    MemoryPool::large(largeUsed, largeFree);


    out_ << name_ << " <= resident size: "
         << eckit::Bytes(rss);

    if ( rss > rss_) {
        out_ << " (+" << eckit::Bytes(rss - rss_) << ")";
    }

    if ( rss < rss_) {
        out_ << " (-" << eckit::Bytes(rss_ - rss) << ")";
    }

    out_ << ", allocated: "
         << eckit::Bytes(malloc);

    if ( malloc > malloc_) {
        out_ << " (+" << eckit::Bytes(malloc - malloc_) << ")";
    }

    if ( malloc < malloc_) {
        out_ << " (-" << eckit::Bytes(malloc_ - malloc) << ")";
    }

    if (shared_ || shared) {
        out_ << ", shared: "
             << eckit::Bytes(shared);

        if ( shared > shared_) {
            out_ << " (+" << eckit::Bytes(shared - shared_) << ")";
        }

        if ( shared < shared_) {
            out_ << " (-" << eckit::Bytes(shared_ - shared) << ")";
        }
    }


//========================
    if (largeUsed_ || largeFree_ || largeUsed || largeFree) {
        out_ << ", large [used: "
             << eckit::Bytes(largeUsed);

        if ( largeUsed > largeUsed_) {
            out_ << " (+" << eckit::Bytes(largeUsed - largeUsed_) << ")";
        }

        if ( largeUsed < largeUsed_) {
            out_ << " (-" << eckit::Bytes(largeUsed_ - largeUsed) << ")";
        }

        out_ << ", free: "
             << eckit::Bytes(largeFree);

        if ( largeFree > largeFree_) {
            out_ << " (+" << eckit::Bytes(largeFree - largeFree_) << ")";
        }

        if ( largeFree < largeFree_) {
            out_ << " (-" << eckit::Bytes(largeFree_ - largeFree) << ")";
        }

        out_ << "]";
    }
    //========================
    if (transientUsed_ || transientFree_ || transientUsed || transientFree) {
        out_ << "transient [used: "
             << eckit::Bytes(transientUsed);

        if ( transientUsed > transientUsed_) {
            out_ << " (+" << eckit::Bytes(transientUsed - transientUsed_) << ")";
        }

        if ( transientUsed < transientUsed_) {
            out_ << " (-" << eckit::Bytes(transientUsed_ - transientUsed) << ")";
        }


        out_ << ", free: "
             << eckit::Bytes(transientFree);

        if ( transientFree > transientFree_) {
            out_ << " (+" << eckit::Bytes(transientFree - transientFree_) << ")";
        }

        if ( transientFree < transientFree_) {
            out_ << " (-" << eckit::Bytes(transientFree_ - transientFree) << ")";
        }
        out_ << "]";
    }

//========================
    if (permanentUsed_ || permanentFree_ || permanentUsed || permanentFree) {

        out_ << ", permanent [used: "
             << eckit::Bytes(permanentUsed);

        if ( permanentUsed > permanentUsed_) {
            out_ << " (+" << eckit::Bytes(permanentUsed - permanentUsed_) << ")";
        }

        if ( permanentUsed < permanentUsed_) {
            out_ << " (-" << eckit::Bytes(permanentUsed_ - permanentUsed) << ")";
        }


        out_ << ", free: "
             << eckit::Bytes(permanentFree);

        if ( permanentFree > permanentFree_) {
            out_ << " (+" << eckit::Bytes(permanentFree - permanentFree_) << ")";
        }

        if ( permanentFree < permanentFree_) {
            out_ << " (-" << eckit::Bytes(permanentFree_ - permanentFree) << ")";
        }

//========================

        out_ << "]";
    }
    out_ << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
