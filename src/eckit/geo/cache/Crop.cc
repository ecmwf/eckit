// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/Crop.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/serialisation/FileStream.h"


namespace eckit::geo::cache {


MemoryUsage Crop::footprint() const {
    return {sizeof(*this) + indices.capacity() * sizeof(renumber_type::value_type), 0};
}


void Crop::read(const PathName& path) {
    FileStream f(path, "r");
    auto close = closer(f);

    unsigned long long size = 0;
    f >> size;

    indices.resize(size);
    if (size > 0) {
        f.readLargeBlob(indices.data(), size * sizeof(renumber_type::value_type));
    }
}


void Crop::write(const PathName& path) const {
    FileStream f(path, "w");
    auto close = closer(f);

    f << static_cast<unsigned long long>(indices.size());
    if (!indices.empty()) {
        f.writeLargeBlob(indices.data(), indices.size() * sizeof(renumber_type::value_type));
    }
}


}  // namespace eckit::geo::cache
