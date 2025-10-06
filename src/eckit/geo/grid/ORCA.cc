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
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
}


namespace eckit::geo::grid {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


const ORCA::ORCARecord& orca_record(const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    static cache::MemoryCacheT<PathName, ORCA::ORCARecord> cache;
    static cache::Download download(LibEcKitGeo::cacheDir() + "/grid/orca");

    auto url  = spec.get_string("url_prefix", "") + spec.get_string("url");
    auto path = download.to_cached_path(url, spec.get_string("uid", ""), ".ek");
    ASSERT_MSG(path.exists(), "ORCA: file '" + path + "' not found");

    if (cache.contains(path)) {
        return cache[path];
    }

    // read and check against metadata (if present)
    auto& record = cache[path];
    record.read(path);
    record.check(spec);

    return record;
}


}  // namespace


ORCA::ORCA(const Spec& spec) :
    Regular(spec),
    name_(spec.get_string("name")),
    arrangement_(arrangement_from_string(spec.get_string("orca_arrangement"))),
    record_(orca_record(spec)),
    container_(new container::PointsLonLatReference{record_.longitudes_, record_.latitudes_}) {
    ASSERT(container_);

    if (spec.has("orca_uid")) {
        reset_uid(spec.get_string("orca_uid"));
    }
}


ORCA::ORCA(uid_t uid) : ORCA(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


ORCA::ORCA(const std::string& name, Arrangement a) :
    ORCA(*std::unique_ptr<Spec>(
        GridFactory::make_spec(spec::Custom({{"grid", name + '_' + arrangement_to_string(a)}})))) {}


std::string ORCA::arrangement() const {
    return arrangement_to_string(arrangement_);
}


Grid::uid_t ORCA::ORCARecord::calculate_uid(Arrangement arrangement) const {
    MD5 hash;
    hash.add(arrangement_to_string(arrangement));

    util::hash_vector_double(hash, latitudes_);
    util::hash_vector_double(hash, longitudes_);

    auto d = hash.digest();
    ASSERT(Grid::is_uid(d));

    return {d};
}


ORCA::ORCARecord::bytes_t ORCA::ORCARecord::footprint() const {
    return sizeof(dimensions_.front()) * dimensions_.size() + sizeof(halo_.front()) * halo_.size() +
           sizeof(pivot_.front()) * pivot_.size() + sizeof(longitudes_.front()) * longitudes_.size() +
           sizeof(latitudes_.front()) * latitudes_.size() + sizeof(flags_.front()) * flags_.size();
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
        ASSERT(Grid::is_uid(uid));
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
    return iterator{new geo::iterator::Unstructured(
        *this, 0, std::make_shared<container::PointsLonLatReference>(record_.longitudes_, record_.latitudes_))};
}


Grid::iterator ORCA::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Grid::uid_t ORCA::calculate_uid() const {
    return record_.calculate_uid(arrangement_);
}


Point ORCA::first_point() const {
    ASSERT(!empty());
    return PointLonLat{record_.longitudes_.front(), record_.latitudes_.front()};
}


Point ORCA::last_point() const {
    ASSERT(!empty());
    return PointLonLat{record_.longitudes_.back(), record_.latitudes_.back()};
}


std::vector<Point> ORCA::to_points() const {
    std::vector<Point> p;
    p.reserve(size());

    for (size_t i = 0; i < size(); ++i) {
        p.emplace_back(PointLonLat{record_.longitudes_[i], record_.latitudes_[i]});
    }
    return p;
}


std::pair<std::vector<double>, std::vector<double>> ORCA::to_latlons() const {
    return {record_.latitudes_, record_.longitudes_};
}


const Grid::order_type& ORCA::order() const {
    NOTIMP;
}


Reordering ORCA::reorder(const order_type& to) const {
    NOTIMP;
}


Spec* ORCA::spec_from_uid(const uid_t& uid) {
    return GridSpecByUID::instance().get(uid).spec();
}


Arrangement ORCA::arrangement_from_string(const std::string& str) {
    return str == "F"   ? Arrangement::ORCA_F
           : str == "T" ? Arrangement::ORCA_T
           : str == "U" ? Arrangement::ORCA_U
           : str == "V" ? Arrangement::ORCA_V
           : str == "W" ? Arrangement::ORCA_W
                        : throw SeriousBug("ORCA: unsupported arrangement '" + str + "'");
}


std::string ORCA::arrangement_to_string(Arrangement a) {
    return a == Arrangement::ORCA_F   ? "F"
           : a == Arrangement::ORCA_T ? "T"
           : a == Arrangement::ORCA_U ? "U"
           : a == Arrangement::ORCA_V ? "V"
           : a == Arrangement::ORCA_W
               ? "W"
               : throw SeriousBug("ORCA: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


void ORCA::fill_spec(spec::Custom& custom) const {
    custom.set("grid", name_ + "_" + arrangement_to_string(arrangement_));
    if (auto _uid = uid(); !GridSpecByUID::instance().exists(_uid)) {
        custom.set("uid", _uid);
    }
}


const std::string& ORCA::type() const {
    static const std::string type{"orca"};
    return type;
}


static const GridRegisterType<ORCA> GRIDTYPE("ORCA");


}  // namespace eckit::geo::grid
