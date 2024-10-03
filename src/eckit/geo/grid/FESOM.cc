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


#include "eckit/geo/grid/FESOM.h"

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


CacheT<PathName, FESOM::FESOMRecord> CACHE;


const FESOM::FESOMRecord& fesom_record(const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    static Download download(LibEcKitGeo::cacheDir() + "/grid/fesom");

    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
    auto path = download.to_cached_path(url, spec.get_string("fesom_name"), ".ekc");
    ASSERT_MSG(path.exists(), "FESOM: file '" + path + "' not found");

    if (CACHE.contains(path)) {
        return CACHE[path];
    }

    // read and check uid
    auto& record = CACHE[path];
    record.read(path);

    return record;
}


}  // namespace


FESOM::FESOM(const Spec& spec) :
    Unstructured(spec),
    uid_(spec.get_string("fesom_uid")),
    arrangement_(arrangement_from_string(spec.get_string("fesom_arrangement"))),
    record_(fesom_record(spec)) {
    resetContainer(new container::LonLatReference{record_.longitudes_, record_.latitudes_});
    ASSERT(container());

    if (spec.get_bool("fesom_uid_check", false)) {
        auto uid = spec.get_string("fesom_uid");
        ASSERT(uid.length() == 32);
        ASSERT(uid == calculate_uid());
    }
}


FESOM::FESOM(uid_t uid) : FESOM(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


std::string FESOM::arrangement() const {
    return arrangement_to_string(arrangement_);
}


FESOM::FESOMRecord::bytes_t FESOM::FESOMRecord::footprint() const {
    return sizeof(longitudes_.front()) * longitudes_.size() + sizeof(latitudes_.front()) * latitudes_.size();
}


size_t FESOM::FESOMRecord::n() const {
    return latitudes_.size();
}


void FESOM::FESOMRecord::read(const PathName& p) {
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

    throw SeriousBug("FESOM: unsupported version", Here());
}


Grid::uid_t FESOM::calculate_uid() const {
    MD5 hash;

    if (arrangement_ == Arrangement::FESOM_N) {
        util::hash_coordinate(hash, record_.latitudes_, !eckit_LITTLE_ENDIAN);
        util::hash_coordinate(hash, record_.longitudes_, !eckit_LITTLE_ENDIAN);
    }
    else if (arrangement_ == Arrangement::FESOM_C) {
        NOTIMP;
    }
    else {
        NOTIMP;
    }

    auto d = hash.digest();
    ASSERT(d.length() == 32);

    return {d};
}


Spec* FESOM::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


void FESOM::fill_spec(spec::Custom& custom) const {
    custom.set("type", "FESOM");
    custom.set("uid", uid());
}


static const GridRegisterType<FESOM> GRIDTYPE("FESOM");
// static const GridRegisterName<FESOM> GRIDNAME(GridRegisterName<FESOM>::uid_pattern);


}  // namespace eckit::geo::grid
