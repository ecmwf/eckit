// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/MappedAnonymousMemory.h"

#include <sys/mman.h>

#include <ostream>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"


namespace eckit::cache::loader {


static const LoaderBuilder<MappedAnonymousMemory> BUILDER("mapped-anonymous-memory");


MappedAnonymousMemory::MappedAnonymousMemory(const PathName& path, const Options&) :
    Loader(path), size_(static_cast<size_t>(path.size())) {
    Timer timer("MappedAnonymousMemory: load '" + path.asString() + "'", Log::debug<LibEcKitCache>());

    if (size_ == 0) {
        return;
    }

    auto* address = ::mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (address == MAP_FAILED) {
        throw FailedSystemCall("mmap (anonymous) for '" + path.asString() + "'", Here());
    }

    try {
        read(path, address, size_);
    }
    catch (...) {
        ::munmap(address, size_);
        throw;
    }

    address_ = address;

    // read-only, as the other loaders
    ::mprotect(address_, size_, PROT_READ);
}


MappedAnonymousMemory::~MappedAnonymousMemory() {
    if (address_ != nullptr && ::munmap(address_, size_) != 0) {
        Log::warning() << "MappedAnonymousMemory: failed to unmap '" << path() << "'" << std::endl;
    }
}


void MappedAnonymousMemory::print(std::ostream& out) const {
    out << "MappedAnonymousMemory[path=" << path() << ",size=" << size_ << "]";
}


}  // namespace eckit::cache::loader
