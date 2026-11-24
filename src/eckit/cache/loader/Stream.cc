// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/Stream.h"

#include <ostream>

#include "eckit/cache/LibEcKitCache.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/serialisation/FileStream.h"


namespace eckit::cache::loader {


static const LoaderBuilder<Stream> BUILDER("stream");


Stream::Stream(const PathName& path, const Options&) : Loader(path) {
    Timer timer("Stream: load '" + path.asString() + "'", Log::debug<LibEcKitCache>());

    FileStream f(path, "r");
    auto close = closer(f);

    unsigned long long size = 0;
    f >> size;

    size_ = static_cast<size_t>(size);
    if (size_ > 0) {
        buffer_.reset(new char[size_]);  // uninitialised, as read
        f.readLargeBlob(buffer_.get(), size_);
    }
}


void Stream::write(const PathName& path, const void* address, size_t size) {
    FileStream f(path, "w");
    auto close = closer(f);

    f << static_cast<unsigned long long>(size);
    if (size > 0) {
        f.writeLargeBlob(address, size);
    }
}


void Stream::print(std::ostream& out) const {
    out << "Stream[path=" << path() << ",size=" << size_ << "]";
}


}  // namespace eckit::cache::loader
