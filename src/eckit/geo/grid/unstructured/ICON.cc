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


#include "eckit/geo/grid/unstructured/ICON.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "eckit/codec/codec.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Cache.h"
#include "eckit/geo/Download.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/container/PointsContainer.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::grid::unstructured {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


const ICON::ICONRecord& icon_record(const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    static CacheT<PathName, ICON::ICONRecord> cache;
    static Download download(LibEcKitGeo::cacheDir() + "/grid/icon");

    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
    auto path = download.to_cached_path(url, spec.get_string("name", ""), ".ek");
    ASSERT_MSG(path.exists(), "ICON: file '" + path + "' not found");

    if (cache.contains(path)) {
        return cache[path];
    }

    // read and check uid
    auto& record = cache[path];
    record.read(path);

    return record;
}


}  // namespace


ICON::ICON(const Spec& spec) :
    Unstructured(spec),
    name_(spec.get_string("name")),
    arrangement_(arrangement_from_string(spec.get_string("icon_arrangement"))),
    record_(icon_record(spec)) {
    resetContainer(new container::PointsLonLatReference{record_.longitudes_, record_.latitudes_});
    ASSERT(container());

    if (spec.has("icon_uid")) {
        reset_uid(spec.get_string("icon_uid"));
    }
}


ICON::ICON(uid_t uid) : ICON(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


ICON::ICON(const std::string& name, Arrangement a) :
    ICON(*std::unique_ptr<Spec>(
        GridFactory::make_spec(spec::Custom({{"grid", name + '_' + arrangement_to_string(a)}})))) {}


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
}


Spec* ICON::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


Arrangement ICON::arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::ICON_C
           : str == "T" ? Arrangement::ICON_T
           : str == "N" ? Arrangement::ICON_N
           : str == "E" ? Arrangement::ICON_E
                        : throw SeriousBug("ICON: unsupported arrangement '" + str + "'");
}


std::string ICON::arrangement_to_string(Arrangement a) {
    return a == Arrangement::ICON_C   ? "C"
           : a == Arrangement::ICON_T ? "T"
           : a == Arrangement::ICON_N ? "N"
           : a == Arrangement::ICON_E
               ? "E"
               : throw SeriousBug("ICON: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


void ICON::fill_spec(spec::Custom& custom) const {
    custom.set("type", "ICON");
    custom.set("uid", uid());
}


const std::string& ICON::type() const {
    static const std::string type{"icon"};
    return type;
}


static const GridRegisterType<ICON> GRIDTYPE("ICON");


}  // namespace eckit::geo::grid::unstructured
