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
void hash_vector_double(MD5&, const std::vector<double>&);
void hash_vector_size_t(MD5&, const std::vector<size_t>&);
}  // namespace eckit::geo::util


namespace eckit::geo::grid {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


const FESOM::FESOMRecord& fesom_record(const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    static CacheT<PathName, FESOM::FESOMRecord> cache;
    static Download download(LibEcKitGeo::cacheDir() + "/grid/fesom");

    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
    auto path = download.to_cached_path(url, spec.get_string("name", ""), ".ek");
    ASSERT_MSG(path.exists(), "FESOM: file '" + path + "' not found");

    if (cache.contains(path)) {
        return cache[path];
    }

    // read and check uid
    auto& record = cache[path];
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


FESOM::FESOM(const std::string& name, const std::string& arrangement) :
    FESOM(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"grid", name + '_' + arrangement}})))) {}


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

    switch (arrangement_) {
    case Arrangement::FESOM_N:
        util::hash_vector_double(hash, record_.latitudes_);
        util::hash_vector_double(hash, record_.longitudes_);
        break;

    case Arrangement::FESOM_C:
        NOTIMP;
        break;

    default:
        NOTIMP;
        break;
    }

    auto d = hash.digest();
    ASSERT(d.length() == 32);

    return {d};
}


Spec* FESOM::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


Arrangement FESOM::arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::FESOM_C
           : str == "N" ? Arrangement::FESOM_N
                        : throw SeriousBug("FESOM: unsupported arrangement '" + str + "'");
}


std::string FESOM::arrangement_to_string(Arrangement a) {
    return a == Arrangement::FESOM_C ? "C"
           : a == Arrangement::FESOM_N
               ? "N"
               : throw SeriousBug("ORCA: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


void FESOM::fill_spec(spec::Custom& custom) const {
    custom.set("type", "FESOM");
    custom.set("uid", uid());
}


static const GridRegisterType<FESOM> GRIDTYPE("FESOM");


}  // namespace eckit::geo::grid
