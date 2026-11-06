// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/MappedFile.h"

#include <sys/mman.h>
#include <unistd.h>

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"


namespace eckit::cache::loader {


static const LoaderBuilder<MappedFile> BUILDER_1("mapped-file");
static const LoaderBuilder<MappedFile> BUILDER_2("mmap");


MappedFile::MappedFile(const PathName& path, const Options&) : Loader(path), size_(static_cast<size_t>(path.size())) {
    if (size_ == 0) {
        return;
    }

    const int fd  = open(path, size_);
    auto* address = ::mmap(nullptr, size_, PROT_READ, MAP_SHARED, fd, 0);
    ::close(fd);  // the mapping remains

    if (address == MAP_FAILED) {
        throw FailedSystemCall("mmap '" + path.asString() + "'", Here());
    }

    address_ = address;
}


MappedFile::~MappedFile() {
    if (address_ != nullptr && ::munmap(address_, size_) != 0) {
        Log::warning() << "MappedFile: failed to unmap '" << path() << "'" << std::endl;
    }
}


void MappedFile::print(std::ostream& out) const {
    out << "MappedFile[path=" << path() << ",size=" << size_ << "]";
}


}  // namespace eckit::cache::loader
