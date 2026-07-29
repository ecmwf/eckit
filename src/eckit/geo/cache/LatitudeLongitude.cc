/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/cache/LatitudeLongitude.h"

#include <memory>
#include <string>

#include "eckit/codec/codec.h"
#include "eckit/config/Resource.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::cache {


namespace {


util::recursive_mutex MUTEX;


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
            static cache::Download download(PathName{LibEcKitGeo::cacheDir()} / "latlon" /
                                            std::to_string(cache::Download::version()));

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
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    return cache().get(uid);
}


const LatitudeLongitude& LatitudeLongitude::set(const uid_type& uid, LatitudeLongitude&& ll) {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
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
    auto uid  = grid::Unstructured::uid_from_latlons(latitude(), longitude());
    auto path = PathName(LibEcKitGeo::cacheDir()) / "latlon" / (uid + ".ek");

    if (auto dir = path.dirName(); !dir.exists()) {
        try {
            // use umask = 0 so mkdir creates the directory with the correct permissions
            AutoUmask umask(0);
            dir.mkdir(0777);
        }
        catch (FailedSystemCall&) {
            // possibly failing later is more meaningful
        }
    }

    if (!path.exists()) {
        write(path);
    }

    return path;
}


size_t LatitudeLongitude::size() const {
    return lat_.size();
}


}  // namespace eckit::geo::cache
