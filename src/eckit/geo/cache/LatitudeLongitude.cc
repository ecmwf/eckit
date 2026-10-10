// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/LatitudeLongitude.h"

#include <memory>
#include <string>
#include <utility>

#include "eckit/codec/codec.h"
#include "eckit/config/Resource.h"
#include "eckit/container/CacheManager.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/Version.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::cache {


namespace {


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

        std::unique_ptr<const Grid::Spec> spec(GridSpecByUID::instance().get(uid).spec());
        ASSERT(spec);

        if (spec->has("cached_path")) {
            record.read(spec->get_string("cached_path"));
            return;
        }

        if (spec->has("url")) {
            static const Download download;
            record.read(download.to_cached_path(LibEcKitGeo::url(spec->get_string("url")), "", ".ek"));
            return;
        }

        throw exception::GridError("LatitudeLongitude: unknown uid='" + uid + "', no 'cached_path' or 'url'", Here());
    }
};


Cache& cache() {
    static const Loader loader;
    static Cache CACHE(loader);
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
    if (static const std::string cmp =
            LibResource<std::string, LibEcKitGeo>("eckit-codec-compression;$ECKIT_CODEC_COMPRESSION", "lz4");
        cmp.empty()) {
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
    // the cache file (as Download, not in memory), created exclusively across processes (eckit::CacheManager: the
    // creation is locked, written to a process-specific file then renamed, so other processes never read a partial
    // file)
    struct Traits {
        using value_type = PathName;
        using Locker     = CacheManagerFileFlock;

        static const char* name() { return "latlon"; }
        static int version() { return version::LATLON; }
        static const char* extension() { return ".ek"; }

        static void save(const CacheManagerBase&, const value_type&, const PathName&) {}  // by the creator
        static void load(const CacheManagerBase&, value_type& path, const PathName& cached) { path = cached; }
    };

    using LatLonCache = CacheManager<Traits>;
    static const LatLonCache CACHE(Traits::name(), LibEcKitGeo::cacheDir(), false, 0);

    struct Creator final : LatLonCache::CacheContentCreator {
        explicit Creator(const LatitudeLongitude& ll) : ll_(ll) {}
        void create(const PathName& path, PathName&, bool& saved) override {
            ll_.write(path);
            saved = true;
        }
        const LatitudeLongitude& ll_;
    } creator(*this);

    // the file lock is per process
    lock_type lock;

    PathName path;
    CACHE.getOrCreate(grid::Unstructured::uid_from_latlons(latitude(), longitude()), creator, path);
    return path;
}


size_t LatitudeLongitude::size() const {
    return lat_.size();
}


}  // namespace eckit::geo::cache
