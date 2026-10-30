// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/Crop.h"

#include <memory>

#include "eckit/cache/FileCache.h"
#include "eckit/cache/loader/Stream.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/cache/CropCache.h"


namespace eckit::geo::cache {


eckit::cache::MemoryUsage Crop::footprint() const {
    return {sizeof(*this) + indices.capacity() * sizeof(renumber_type::value_type), 0};
}


void Crop::read(const PathName& path) {
    std::unique_ptr<eckit::cache::Loader> loader(CropCache::file_cache().load("stream", path));

    const auto* begin = static_cast<const renumber_type::value_type*>(loader->address());
    indices.assign(begin, begin + loader->size() / sizeof(renumber_type::value_type));
}


void Crop::write(const PathName& path) const {
    eckit::cache::loader::Stream::write(path, indices.data(), indices.size() * sizeof(renumber_type::value_type));
}


}  // namespace eckit::geo::cache
