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
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/io/Length.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/ByteSwap.h"
#include "eckit/utils/MD5.h"

#if eckit_HAVE_CURL
#include "eckit/io/URLHandle.h"
#endif


namespace eckit::geo::grid {


namespace {


ORCA::Arrangement arrangement_from_string(const std::string& str) {
    return str == "F"   ? ORCA::Arrangement::F
           : str == "T" ? ORCA::Arrangement::T
           : str == "U" ? ORCA::Arrangement::U
           : str == "V" ? ORCA::Arrangement::V
           : str == "W" ? ORCA::Arrangement::W
                        : throw AssertionFailed("ORCA::Arrangement", Here());
}


std::string arrangement_to_string(ORCA::Arrangement a) {
    return a == ORCA::Arrangement::F   ? "F"
           : a == ORCA::Arrangement::T ? "T"
           : a == ORCA::Arrangement::U ? "U"
           : a == ORCA::Arrangement::V ? "V"
           : a == ORCA::Arrangement::W ? "W"
                                       : throw AssertionFailed("ORCA::Arrangement", Here());
}


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


CacheT<PathName, ORCA::ORCARecord> CACHE;


PathName orca_path(const PathName& path, const std::string& url) {
    // control concurrent download/access
    lock_type lock;

#if eckit_HAVE_CURL  // for eckit::URLHandle
    if (!path.exists() && LibEcKitGeo::caching()) {
        auto dir = path.dirName();
        dir.mkdir();
        ASSERT(dir.exists());

        auto tmp = path + ".download";

        Timer timer;
        Log::info() << "ORCA: downloading '" << url << "' to '" << path << "'..." << std::endl;

        Length length = 0;
        try {
            length = URLHandle(url).saveInto(tmp);
        }
        catch (...) {
            length = 0;
        }

        if (length <= 0) {
            if (tmp.exists()) {
                tmp.unlink(true);
            }

            throw UserError("ORCA: download error", Here());
        }

        PathName::rename(tmp, path);
        Log::info() << "ORCA: download of " << Bytes(static_cast<double>(length)) << " took " << timer.elapsed()
                    << " s." << std::endl;
    }
#endif

    ASSERT_MSG(path.exists(), "ORCA: file '" + path + "' not found");
    return path;
}


const ORCA::ORCARecord& orca_record(const PathName& p, const Spec& spec) {
    // control concurrent reads/writes
    lock_type lock;

    if (CACHE.contains(p)) {
        return CACHE[p];
    }

    // read and check against metadata (if present)
    auto& record = CACHE[p];
    record.read(p);
    record.check(spec);

    return record;
}


}  // namespace


ORCA::ORCA(const Spec& spec) :
    Regular(spec),
    name_(spec.get_string("orca_name")),
    uid_(spec.get_string("orca_uid")),
    arrangement_(arrangement_from_string(spec.get_string("orca_arrangement"))),
    record_(orca_record(
        orca_path(spec.get_string("path", LibEcKitGeo::cacheDir() + "/eckit/geo/grid/orca/" + uid_ + ".atlas"),
                  spec.get_string("url_prefix", "") + spec.get_string("url")),
        spec)) {}


ORCA::ORCA(uid_t uid) : ORCA(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


std::string ORCA::arrangement() const {
    return arrangement_to_string(arrangement_);
}


Grid::uid_t ORCA::ORCARecord::calculate_uid(Arrangement arrangement) const {
    MD5 hash;
    hash.add(arrangement_to_string(arrangement));

    auto sized = static_cast<long>(longitudes_.size() * sizeof(double));

    if constexpr (eckit_LITTLE_ENDIAN) {
        hash.add(latitudes_.data(), sized);
        hash.add(longitudes_.data(), sized);
    }
    else {
        auto lonsw = longitudes_;
        auto latsw = latitudes_;
        eckit::byteswap(latsw.data(), latsw.size());
        eckit::byteswap(lonsw.data(), lonsw.size());
        hash.add(latsw.data(), sized);
        hash.add(lonsw.data(), sized);
    }

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
    MD5 hash(arrangement_to_string(arrangement_));

    if (const auto len = static_cast<long>(size() * sizeof(double)); eckit_LITTLE_ENDIAN) {
        hash.add(record_.latitudes_.data(), len);
        hash.add(record_.longitudes_.data(), len);
    }
    else {
        auto ll = to_latlon();
        byteswap(ll.first.data(), size());
        byteswap(ll.second.data(), size());

        hash.add(ll.first.data(), len);
        hash.add(ll.second.data(), len);
    }

    return hash;
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
