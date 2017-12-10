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

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

MemoryInfo::MemoryInfo() {
    ::memset(this, 0, sizeof(*this));

    MemoryPool::info(transientUsed_, transientFree_, MemPool::transientPool);
    MemoryPool::info(permanentUsed_, permanentFree_, MemPool::permanentPool);
    MemoryPool::large(largeUsed_, largeFree_);


}

void MemoryInfo::print(std::ostream&) const {
}

void MemoryInfo::delta(std::ostream&, const MemoryInfo& other) const {
}


} // namespace system
} // namespace eckit

