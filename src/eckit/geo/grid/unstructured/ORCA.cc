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


#include "eckit/geo/grid/unstructured/ORCA.h"

#include "eckit/codec/codec.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/io/Length.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/ByteSwap.h"
#include "eckit/utils/MD5.h"

#ifdef eckit_HAVE_CURL
#include "eckit/io/URLHandle.h"
#endif


namespace eckit::geo::grid::unstructured {


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


}  // namespace


ORCA::ORCA(const Configuration& config) :
    Unstructured(config),
    name_(config.getString("orca_name")),
    uid_(config.getString("orca_uid")),
    arrangement_(arrangement_from_string(config.getString("orca_arrangement"))),
    dimensions_{-1, -1},
    halo_{-1, -1, -1, -1},
    pivot_{-1, -1} {
    PathName path = config.getString("path", LibEcKitGeo::cacheDir() + "/eckit/geo/orca/" + uid_ + ".atlas");

#if eckit_HAVE_CURL  // for eckit::URLHandle
    if (!path.exists() && LibEcKitGeo::caching()) {
        auto dir = path.dirName();
        dir.mkdir();
        ASSERT(dir.exists());

        auto tmp = path + ".download";
        auto url = config.getString("url_prefix", "") + config.getString("url");

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

            throw UserError("ORCA: download error");
        }

        PathName::rename(tmp, path);
        Log::info() << "ORCA: download of " << Bytes(static_cast<double>(length)) << " took " << timer.elapsed()
                    << " s." << std::endl;
    }
#endif

    ASSERT_MSG(path.exists(), "ORCA: file '" + path + "' not found");

    // read and check against metadata (if present)
    read(path);
    check(config);
}


Grid::uid_t ORCA::calculate_uid() const {
    MD5 hash;
    hash.add(arrangement_to_string(arrangement_));

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


void ORCA::read(const PathName& p) {
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

    throw SeriousBug("ORCA::read: unsupported version");
}


void ORCA::check(const Configuration& config) {
    ASSERT(uid_.length() == 32);
    if (config.getBool("orca_uid_check", false)) {
        ASSERT(uid_ == uid());
    }

    if (std::vector<decltype(dimensions_)::value_type> d; config.get("dimensions", d)) {
        ASSERT(d.size() == 2);
        ASSERT(d[0] == dimensions_[0] && d[1] == dimensions_[1]);
    }

    if (std::vector<decltype(halo_)::value_type> h; config.get("halo", h)) {
        ASSERT(h.size() == 4);
        ASSERT(h[0] == halo_[0] && h[1] == halo_[1] && h[2] == halo_[2] && h[3] == halo_[3]);
    }

    if (std::vector<decltype(pivot_)::value_type> p; config.get("pivot", p)) {
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


size_t ORCA::write(const PathName& p, const std::string& compression) {
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
    return iterator{new geo::iterator::Unstructured(*this, 0)};
}


Grid::iterator ORCA::cend() const {
    return iterator{new geo::iterator::Unstructured(*this, size())};
}


const area::BoundingBox& ORCA::boundingBox() const {
    static const auto __bbox(area::BoundingBox::make_global_prime());
    return __bbox;
}


std::pair<std::vector<double>, std::vector<double>> ORCA::to_latlon() const {
    return {latitudes_, longitudes_};
}


Configuration* ORCA::config(const std::string& name) {
    return GridConfigurationUID::instance().get(name).config();
}


static const GridRegisterType<ORCA> __grid_type("ORCA");
static const GridRegisterName<ORCA> __grid_pattern(GridRegisterName<ORCA>::uid_pattern);


}  // namespace eckit::geo::grid::unstructured