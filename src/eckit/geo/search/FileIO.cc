// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/FileIO.h"

#include <fstream>
#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Trace.h"


namespace eckit::geo::search {


static const KDTreeBuilder<FileIO> BUILDER("file-io");


FileIO::FileIO(const std::string& uid, size_t size) : LoadedFile(uid, size) {}


KDTree::MemoryUsage FileIO::footprint() const {
    return loaded() ? MemoryUsage{buffer_size_, 0} : LoadedFile::footprint();
}


char* FileIO::load() {
    const auto name = path().asString();
    Trace trace("FileIO: load '" + name + "'");

    const auto size = storage_size(this->size());    // as validated (see MappedFile::ready)
    std::unique_ptr<char[]> buffer(new char[size]);  // uninitialised, as read

    if (std::ifstream in(name, std::ios::binary); !in.read(buffer.get(), static_cast<std::streamsize>(size))) {
        throw exception::SearchError("FileIO: failed to read '" + name + "'", Here());
    }

    buffer_      = std::move(buffer);
    buffer_size_ = size;
    return buffer_.get();
}


void FileIO::print(std::ostream& out) const {
    out << "FileIO[path=" << path() << ",size=" << size() << ",loaded=" << loaded() << "]";
}


}  // namespace eckit::geo::search
