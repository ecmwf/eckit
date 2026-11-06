// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/File.h"

#include <ostream>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"


namespace eckit::cache::loader {


static const LoaderBuilder<File> BUILDER("file");


File::File(const PathName& path, const Options&) : Loader(path), size_(static_cast<size_t>(path.size())) {
    Timer timer("File: load '" + path.asString() + "'", Log::debug<LibEcKitCache>());

    if (size_ == 0) {
        return;
    }

    std::unique_ptr<char[]> buffer(new char[size_]);  // uninitialised, as read
    read(path, buffer.get(), size_);

    buffer_ = std::move(buffer);
}


void File::print(std::ostream& out) const {
    out << "File[path=" << path() << ",size=" << size_ << "]";
}


}  // namespace eckit::cache::loader
