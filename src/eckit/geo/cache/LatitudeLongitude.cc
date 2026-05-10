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
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::cache {


using Cache = cache::RecordCacheT<LatitudeLongitude, Grid::uid_type>;


struct Loader final : Cache::Loader {
    void load(const Cache::key_type& uid, Cache::value_type& record) const override {
        static cache::Download download(LibEcKitGeo::cacheDir() + "/latlon");

        if (!GridSpecByUID::instance().exists(uid)) {
            throw exception::GridError("LatitudeLongitude: unknown uid='" + uid + "'", Here());
        }

        const auto url = LibEcKitGeo::url(
            std::unique_ptr<const Grid::Spec>(GridSpecByUID::instance().get(uid).spec())->get_string("url"));

        const auto path = download.to_cached_path(url, "", ".ek");
        ASSERT_MSG(path.exists(), "LatitudeLongitude: file '" + path + "' not found (url: '" + url + "')");

        record.read(path);
    }
};


const LatitudeLongitude& LatitudeLongitude::get(const Grid::uid_type& uid) {
    static const Loader loader;
    static Cache cache(loader);
    return cache.get(uid);
}


void LatitudeLongitude::read(const PathName& p) {
    codec::RecordReader reader(p);

    reader.read("latitude", latitude_);
    reader.read("longitude", longitude_);
    reader.wait();

    ASSERT(latitude_.size() == longitude_.size());
}


size_t LatitudeLongitude::size() const {
    return latitude_.size();
}


}  // namespace eckit::geo::cache
