// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/LatitudeLongitude.h"

#include <memory>
#include <string>
#include <utility>

#include "eckit/cache/Download.h"
#include "eckit/cache/FileCache.h"
#include "eckit/codec/codec.h"
#include "eckit/config/Resource.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::cache {


namespace {


// coordinates are loaded once (loading can be a download), not concurrently
class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


using Cache = cache::RecordCacheT<LatitudeLongitude, LatitudeLongitude::uid_type>;


struct Loader final : Cache::Loader {
    Loader() = default;

    void load(const Cache::key_type& uid, Cache::value_type& record) const override {
        if (!GridSpecByUID::instance().exists(uid)) {
            throw exception::GridError("LatitudeLongitude: unknown uid='" + uid + "'", Here());
        }

        std::unique_ptr<const Grid::Spec> spec(GridSpecByUID::instance().get_spec(uid));
        ASSERT(spec);

        if (spec->has("cached_path")) {
            record.read(spec->get_string("cached_path"));
            return;
        }

        if (spec->has("url")) {
            static const eckit::cache::Download download(LibEcKitGeo::cacheRoots() / "download");
            record.read(download.to_cached_path(LibEcKitGeo::url(spec->get_string("url")), "", ".ek"));
            return;
        }

        throw exception::GridError("LatitudeLongitude: unknown uid='" + uid + "', no 'cached_path' or 'url'", Here());
    }
};


Cache& cache() {
    static const Loader loader;
    static Cache CACHE(LibEcKitGeo::memoryCaches(), loader);
    return CACHE;
}


}  // namespace


LatitudeLongitude::LatitudeLongitude(const std::vector<double>& lat, const std::vector<double>& lon) :
    lon_(lon), lat_(lat) {
    ASSERT(lat_.size() == lon_.size());
}


const LatitudeLongitude& LatitudeLongitude::get(const uid_type& uid) {
    lock_type lock;
    return cache().get(uid);
}


const LatitudeLongitude& LatitudeLongitude::set(const uid_type& uid, LatitudeLongitude&& ll) {
    lock_type lock;
    return cache().set(uid, std::move(ll));
}


void LatitudeLongitude::read(const PathName& p) {
    ASSERT_MSG(p.exists(), "LatitudeLongitude: path '" + p + "' not found");

    codec::RecordReader record(p);
    record.read("latitude", lat_);
    record.read("longitude", lon_);
    record.wait();

    ASSERT(lat_.size() == lon_.size());
}


void LatitudeLongitude::write(const PathName& p) const {
    codec::RecordWriter record;
    static const std::string cmp =
        LibResource<std::string, LibEcKitGeo>("eckit-codec-compression;$ECKIT_CODEC_COMPRESSION", "lz4");
    if (cmp.empty()) {
        record.compression(false);
    }
    else {
        record.compression(cmp);
    }

    record.set("latitude", codec::ref(lat_));
    record.set("longitude", codec::ref(lon_));
    record.write(p);
}


PathName LatitudeLongitude::to_cached_path() const {
    const auto uid = grid::Unstructured::uid_from_latlons(latitude(), longitude());
    return file_cache().get_or_create(uid, [this](const PathName& tmp) { write(tmp); });
}


const eckit::cache::FileCache& LatitudeLongitude::file_cache() {
    // written from the grid in memory, so never missing (no throw on miss)
    static const eckit::cache::FileCache cache(LibEcKitGeo::cacheRoots(), "latlon", VERSION, ".ek");
    return cache;
}


size_t LatitudeLongitude::size() const {
    return lat_.size();
}


}  // namespace eckit::geo::cache
