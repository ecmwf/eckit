// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/LatitudeLongitude.h"

#include <map>
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


struct State final : geo::Grid::Iterator::State {
    explicit State(const LatitudeLongitude& record) : record_(record) {}

    Point point(size_t index) override { return record_.point(index); }

    std::unique_ptr<geo::Grid::Iterator::State> clone() const override { return std::make_unique<State>(*this); }

    const LatitudeLongitude& record_;
};


}  // namespace


std::unique_ptr<geo::Grid::Iterator::State> LatitudeLongitude::make_state() const {
    return std::make_unique<State>(*this);
}


namespace {


// coordinates are loaded once (loading can be a download), not concurrently
struct lock_tag;  // of this file's mutex
using lock_type = util::lock_type<lock_tag>;


using Cache = cache::RecordCacheT<LatitudeLongitude, LatitudeLongitude::uid_type>;


std::map<std::string, LatitudeLongitude::load_type>& loads() {
    static std::map<std::string, LatitudeLongitude::load_type> loads;
    return loads;
}


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

        if (auto load = loads().find(spec->get_string("type")); load != loads().end()) {
            record = load->second(*spec);
            return;
        }

        if (spec->has("url")) {
            static const eckit::cache::Download download(LibEcKitGeo::cacheRoots() / "download");
            record.read(download.toCachedPath(LibEcKitGeo::url(spec->get_string("url")), "", ".ek"));
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


LatitudeLongitude::LatitudeLongitude(std::vector<double> lat, std::vector<double> lon) :
    lon_(std::move(lon)), lat_(std::move(lat)) {
    if (lat_.size() != lon_.size()) {
        throw exception::SpecError(
            "LatitudeLongitude: {latitudes: [...], longitudes: [...]} (of the same size) expected", Here());
    }
}


const LatitudeLongitude& LatitudeLongitude::get(const uid_type& uid) {
    lock_type lock;
    return cache().get(uid);
}


const LatitudeLongitude& LatitudeLongitude::set(const uid_type& uid, LatitudeLongitude&& ll) {
    lock_type lock;
    return cache().set(uid, std::move(ll));
}


void LatitudeLongitude::register_load(const std::string& type, load_type load) {
    lock_type lock;
    loads()[type] = load;
}


void LatitudeLongitude::read(const PathName& p) {
    if (!p.exists()) {
        throw exception::GridError("LatitudeLongitude: '" + p + "' not found (e.g. the cache was removed)", Here());
    }

    codec::RecordReader record(p);
    record.read("latitude", lat_);
    record.read("longitude", lon_);
    record.wait();

    if (lat_.size() != lon_.size()) {
        throw exception::GridError("LatitudeLongitude: '" + p + "' latitudes and longitudes of different sizes",
                                   Here());
    }
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
    return file_cache().getOrCreate(uid, [this](const PathName& tmp) { write(tmp); });
}


const eckit::cache::FileCache& LatitudeLongitude::file_cache() {
    // written from the grid in memory, so never missing (no throw on miss)
    static const eckit::cache::FileCache cache(LibEcKitGeo::cacheRoots(), "latlon", VERSION, ".ek", {}, false);
    return cache;
}


size_t LatitudeLongitude::size() const {
    return lat_.size();
}


}  // namespace eckit::geo::cache
