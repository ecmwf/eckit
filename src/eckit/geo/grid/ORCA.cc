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


#include "eckit/geo/grid/ORCA.h"

#include <memory>

#include "eckit/codec/codec.h"
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Cache.h"
#include "eckit/geo/Download.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
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


CacheT<PathName, ORCA::ORCARecord> CACHE;


const ORCA::ORCARecord& orca_record(const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    static Download download(LibEcKitGeo::cacheDir() + "/grid/orca", "orca-", ".codec");

    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
    auto path = download.to_cached_path(url);
    ASSERT_MSG(path.exists(), "ORCA: file '" + path + "' not found");

    if (CACHE.contains(path)) {
        return CACHE[path];
    }

    // read and check against metadata (if present)
    auto& record = CACHE[path];
    record.read(path);
    record.check(spec);

    return record;
}


}  // namespace


ORCA::ORCA(const Spec& spec) :
    Regular(spec),
    name_(spec.get_string("orca_name")),
    uid_(spec.get_string("orca_uid")),
    arrangement_(arrangement_from_string(spec.get_string("orca_arrangement"))),
    record_(orca_record(spec)) {}


ORCA::ORCA(uid_t uid) : ORCA(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


std::string ORCA::arrangement() const {
    return arrangement_to_string(arrangement_);
}


Grid::uid_t ORCA::ORCARecord::calculate_uid(Arrangement arrangement) const {
    MD5 hash;
    hash.add(arrangement_to_string(arrangement));

    util::hash_coordinate(hash, latitudes_, !eckit_LITTLE_ENDIAN);
    util::hash_coordinate(hash, longitudes_, !eckit_LITTLE_ENDIAN);

    auto d = hash.digest();
    ASSERT(d.length() == 32);

    return {d};
}


ORCA::ORCARecord::bytes_t ORCA::ORCARecord::footprint() const {
    return sizeof(dimensions_.front()) * dimensions_.size() + sizeof(halo_.front()) * halo_.size()
           + sizeof(pivot_.front()) * pivot_.size() + sizeof(longitudes_.front()) * longitudes_.size()
           + sizeof(latitudes_.front()) * latitudes_.size() + sizeof(flags_.front()) * flags_.size();
}


size_t ORCA::ORCARecord::ni() const {
    ASSERT(0 <= dimensions_[0]);
    return static_cast<size_t>(dimensions_[0]);
}


size_t ORCA::ORCARecord::nj() const {
    ASSERT(0 <= dimensions_[1]);
    return static_cast<size_t>(dimensions_[1]);
}


void ORCA::ORCARecord::read(const PathName& p) {
    codec::RecordReader reader(p);

    int version = -1;
    reader.read("version", version).wait();

    if (version == 0) {
        reader.read("dimensions", dimensions_);
        reader.read("pivot", pivot_);  // different order from writer
        reader.read("halo", halo_);    //...
        reader.read("longitude", longitudes_);
        reader.read("latitude", latitudes_);
        reader.read("flags", flags_);
        reader.wait();
        return;
    }

    throw SeriousBug("ORCA::read: unsupported version", Here());
}


void ORCA::ORCARecord::check(const Spec& spec) const {
    if (spec.get_bool("orca_uid_check", false)) {
        auto uid = spec.get_string("orca_uid");
        ASSERT(uid.length() == 32);
        ASSERT(uid == calculate_uid(arrangement_from_string(spec.get_string("orca_arrangement"))));
    }

    if (std::vector<decltype(dimensions_)::value_type> d; spec.get("dimensions", d)) {
        ASSERT(d.size() == 2);
        ASSERT(d[0] == dimensions_[0] && d[1] == dimensions_[1]);
    }

    if (std::vector<decltype(halo_)::value_type> h; spec.get("halo", h)) {
        ASSERT(h.size() == 4);
        ASSERT(h[0] == halo_[0] && h[1] == halo_[1] && h[2] == halo_[2] && h[3] == halo_[3]);
    }

    if (std::vector<decltype(pivot_)::value_type> p; spec.get("pivot", p)) {
        ASSERT(p.size() == 2);
        ASSERT(types::is_approximately_equal(p[0], pivot_[0]));
        ASSERT(types::is_approximately_equal(p[1], pivot_[1]));
    }

    auto n = static_cast<size_t>(dimensions_[0] * dimensions_[1]);
    ASSERT(n > 0);
    ASSERT(n == longitudes_.size());
    ASSERT(n == latitudes_.size());
    ASSERT(n == flags_.size());
}


size_t ORCA::ORCARecord::write(const PathName& p, const std::string& compression) {
    codec::RecordWriter record;

    codec::ArrayShape shape{static_cast<size_t>(dimensions_[0]), static_cast<size_t>(dimensions_[1])};

    record.compression(compression);
    record.set("version", 0);
    record.set("dimensions", dimensions_);
    record.set("halo", halo_);
    record.set("pivot", pivot_);
    record.set("longitude", codec::ArrayReference(longitudes_.data(), shape));
    record.set("latitude", codec::ArrayReference(latitudes_.data(), shape));
    record.set("flags", codec::ArrayReference(flags_.data(), shape));

    return record.write(p);
}


Grid::iterator ORCA::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, record_.longitudes_, record_.latitudes_)};
}


Grid::iterator ORCA::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Grid::uid_t ORCA::calculate_uid() const {
    return record_.calculate_uid(arrangement_);
}


std::vector<Point> ORCA::to_points() const {
    std::vector<Point> p;
    p.reserve(size());

    for (size_t i = 0; i < size(); ++i) {
        p.emplace_back(PointLonLat{record_.longitudes_[i], record_.latitudes_[i]});
    }
    return p;
}


std::pair<std::vector<double>, std::vector<double>> ORCA::to_latlon() const {
    return {record_.latitudes_, record_.longitudes_};
}


Spec* ORCA::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


void ORCA::fill_spec(spec::Custom& custom) const {
    custom.set("type", "ORCA");
    custom.set("uid", uid_);
}


static const GridRegisterType<ORCA> GRIDTYPE("ORCA");
static const GridRegisterName<ORCA> GRIDNAME(GridRegisterName<ORCA>::uid_pattern);


}  // namespace eckit::geo::grid
