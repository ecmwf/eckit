// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/MappedFile.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ostream>

#include "eckit/cache/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/memory/MMap.h"
#include "eckit/os/Stat.h"


namespace eckit::cache::loader {


static const LoaderBuilder<MappedFile> BUILDER_1("mapped-file");
static const LoaderBuilder<MappedFile> BUILDER_2("mmap");


namespace {


// Open a file, checking it has the expected size (not replaced since, as mapping beyond its end faults)
int open_file(const PathName& path, size_t size) {
    int fd = -1;
    SYSCALL2(fd = ::open(path.localPath(), O_RDONLY | O_CLOEXEC), path);

    Stat::Struct s;
    if (Stat::fstat(fd, &s) != 0 || static_cast<size_t>(s.st_size) != size) {
        ::close(fd);
        throw CacheError("MappedFile: '" + path.asString() + "' changed while loading", Here());
    }

    return fd;
}


}  // namespace


MappedFile::MappedFile(const PathName& path, const Options&) : Loader(path), size_(static_cast<size_t>(path.size())) {
    if (size_ == 0) {
        return;
    }

    const int fd  = open_file(path, size_);
    auto* address = MMap::mmap(nullptr, size_, PROT_READ, MAP_SHARED, fd, 0);
    ::close(fd);  // the mapping remains

    if (address == MAP_FAILED) {
        throw FailedSystemCall("mmap '" + path.asString() + "'", Here());
    }

    address_ = address;
}


MappedFile::~MappedFile() {
    if (address_ != nullptr && MMap::munmap(address_, size_) != 0) {
        Log::warning() << "MappedFile: failed to unmap '" << path() << "'" << std::endl;
    }
}


void MappedFile::print(std::ostream& out) const {
    out << "MappedFile[path=" << path() << ",size=" << size_ << "]";
}


}  // namespace eckit::cache::loader
