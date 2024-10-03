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


#include "eckit/geo/grid/ICON.h"

#include <memory>
#include <vector>

#include "eckit/codec/codec.h"
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Cache.h"
#include "eckit/geo/Download.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/container/LonLatReference.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_coordinate(MD5&, const std::vector<double>&, bool _byteswap);
}


namespace eckit::geo::grid {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


CacheT<PathName, ICON::ICONRecord> CACHE;


const ICON::ICONRecord& icon_record(const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    static Download download(LibEcKitGeo::cacheDir() + "/grid/icon");

    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
    auto path = download.to_cached_path(url, "icon-", ".codec");
    ASSERT_MSG(path.exists(), "ICON: file '" + path + "' not found");

    if (CACHE.contains(path)) {
        return CACHE[path];
    }

    // read and check uid
    auto& record = CACHE[path];
    record.read(path);

    return record;
}


}  // namespace


ICON::ICON(const Spec& spec) :
    Unstructured(spec),
    uid_(spec.get_string("icon_uid")),
    // arrangement_(arrangement_from_string(spec.get_string("icon_arrangement"))),
    record_(icon_record(spec)) {
    resetContainer(new container::LonLatReference{record_.longitudes_, record_.latitudes_});
    ASSERT(container());

    if (spec.get_bool("icon_uid_check", false)) {
        auto uid = spec.get_string("icon_uid");
        ASSERT(uid.length() == 32);
        ASSERT(uid == calculate_uid());
    }
}


ICON::ICON(uid_t uid) : ICON(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


std::string ICON::arrangement() const {
    return arrangement_to_string(arrangement_);
}


ICON::ICONRecord::bytes_t ICON::ICONRecord::footprint() const {
    return sizeof(longitudes_.front()) * longitudes_.size() + sizeof(latitudes_.front()) * latitudes_.size();
}


size_t ICON::ICONRecord::n() const {
    return latitudes_.size();
}


void ICON::ICONRecord::read(const PathName& p) {
    codec::RecordReader reader(p);

    uint64_t version = 0;
    reader.read("version", version).wait();

    if (version == 0) {
        uint64_t n = 0;
        reader.read("n", n);

        reader.read("latitude", latitudes_);
        reader.read("longitude", longitudes_);
        reader.wait();

        ASSERT(n == latitudes_.size());
        ASSERT(n == longitudes_.size());
        return;
    }

    throw SeriousBug("ICON: unsupported version", Here());
}


Grid::uid_t ICON::calculate_uid() const {
    NOTIMP;
    MD5 hash;

#if 0
    if (arrangement_ == Arrangement::ICON_N) {
        util::hash_coordinate(hash, record_.latitudes_, !eckit_LITTLE_ENDIAN);
        util::hash_coordinate(hash, record_.longitudes_, !eckit_LITTLE_ENDIAN);
    }
    else if (arrangement_ == Arrangement::ICON_C) {
        NOTIMP;
    }
    else {
        NOTIMP;
    }
#endif

    auto d = hash.digest();
    ASSERT(d.length() == 32);

    return {d};
}


Spec* ICON::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


void ICON::fill_spec(spec::Custom& custom) const {
    custom.set("type", "ICON");
    custom.set("uid", uid());
}


static const GridRegisterType<ICON> GRIDTYPE("ICON");
// static const GridRegisterName<ICON> GRIDNAME(GridRegisterName<ICON>::uid_pattern);


}  // namespace eckit::geo::grid
