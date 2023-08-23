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


#include "eckit/geometry/grid/ORCA.h"

#include "eckit/codec/codec.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/geometry/LibEcKitGeometry.h"
#include "eckit/geometry/area/BoundingBox.h"
#include "eckit/io/Length.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/ByteSwap.h"
#include "eckit/utils/MD5.h"

#ifdef eckit_HAVE_CURL
#include "eckit/io/URLHandle.h"
#endif


namespace eckit::geometry::grid {


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


ORCA::Iterator::Iterator(const Grid& grid, size_t index) :
    geometry::Iterator(grid),
    index_(index),
    index_size_(grid.size()),
    longitudes_(dynamic_cast<const ORCA&>(grid).longitudes_),
    latitudes_(dynamic_cast<const ORCA&>(grid).latitudes_),
    uid_(dynamic_cast<const ORCA&>(grid).uid_) {
}


bool ORCA::Iterator::operator==(const geometry::Iterator& other) const {
    const auto* another = dynamic_cast<const Iterator*>(&other);
    return another != nullptr && uid_ == another->uid_;
}


bool ORCA::Iterator::operator++() {
    if (index_++; index_ < index_size_) {
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


bool ORCA::Iterator::operator+=(diff_t d) {
    if (auto di = static_cast<diff_t>(index_); 0 <= di + d && di + d < static_cast<diff_t>(index_size_)) {
        index_ = static_cast<size_t>(di + d);
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


ORCA::Iterator::operator bool() const {
    return index_ < index_size_;
}


Point ORCA::Iterator::operator*() const {
    return PointLonLat{longitudes_.at(index_), latitudes_.at(index_)};
}


ORCA::ORCA(const Configuration& config) :
    Grid(config),
    name_(config.getString("orca_name")),
    uid_(config.getString("orca_uid")),
    arrangement_(arrangement_from_string(config.getString("orca_arrangement"))) {


    // TODO
    ASSERT(0 < dimensions_[0]);
    ASSERT(0 < dimensions_[1]);

    ASSERT(halo_[0] >= 0);
    ASSERT(halo_[1] >= 0);
    ASSERT(halo_[2] >= 0);
    ASSERT(halo_[3] >= 0);

    ASSERT(pivot_[0] >= 0);
    ASSERT(pivot_[1] >= 0);

    size_t size = dimensions_[0] * dimensions_[1];
    ASSERT(0 < size);
    ASSERT(longitudes_.size() == size);
    ASSERT(latitudes_.size() == size);
    ASSERT(flags_.size() == size);


    auto url = config.getString("url_prefix", "") + config.getString("url");
    Log::info() << "url: '" << url << "'" << std::endl;
}


ORCA::ORCA(const URI& uri) :
    Grid(area::BoundingBox::make_global_prime()) {
    if (uri.scheme().find("http") == 0) {
        PathName path = "...";  // from url

        if (!path.exists() && LibEcKitGeometry::caching()) {
            Timer timer;


            Log::debug() << "Downloading " << uri << " to " << path << std::endl;

            path.dirName().mkdir();

            PathName tmp  = path + ".download";
            Length length = 0;

#if eckit_HAVE_CURL
            try {
                length = URLHandle(uri.asRawString()).saveInto(tmp);
            }
            catch (...) {
                length = 0;
            }
#endif

            if (length <= 0) {
                if (tmp.exists()) {
                    tmp.unlink(true);
                }

                throw UserError("Could not download file from url " + uri.asRawString());
            }

            if (!path.exists()) {
                throw UserError("Could not locate orca grid data file " + path);
            }

            PathName::rename(tmp, path);

            Log::info() << "Download of " << Bytes(length) << " took " << timer.elapsed() << " s." << std::endl;
        }
        else {
            throw UserError("Could not locate orca grid data file " + path);
        }
    }
    else {
        if (!uri.path().exists()) {
            throw UserError("Could not locate orca grid data file " + uri.asRawString());
        }
    }
}


void ORCA::read(const PathName& p) {
    codec::RecordReader reader(p);

    int version = -1;
    reader.read("version", version).wait();

    if (version == 0) {
        reader.read("dimensions", dimensions_);
        reader.read("pivot", pivot_);
        reader.read("halo", halo_);
        reader.read("longitude", longitudes_);
        reader.read("latitude", latitudes_);
        reader.read("flags", flags_);
        reader.wait();
    }
    else {
        ASSERT_MSG(false, "Unsupported version ");
    }
}


size_t ORCA::write(const PathName& p, const std::string& compression) {
    codec::RecordWriter record;

    record.compression(compression);
    record.set("version", 0);
    record.set("dimensions", dimensions_);
    record.set("halo", halo_);
    record.set("pivot", pivot_);
    record.set("longitude", codec::ArrayReference(longitudes_.data(), dimensions_));
    record.set("latitude", codec::ArrayReference(latitudes_.data(), dimensions_));
    record.set("flags", codec::ArrayReference(flags_.data(), dimensions_));

    return record.write(p);
}


std::string ORCA::uid(const Configuration& config) const {
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

    return hash.digest();
}


Configuration* ORCA::config(const std::string& name) {
    return GridConfigurationUID::instance().get(name).config();
}


Grid::iterator ORCA::cbegin() const {
    return iterator{new Iterator(*this, 0)};
}


Grid::iterator ORCA::cend() const {
    return iterator{new Iterator(*this, size())};
}


const area::BoundingBox& ORCA::boundingBox() const {
    static const auto __bbox(area::BoundingBox::make_global_prime());
    return __bbox;
}


std::pair<std::vector<double>, std::vector<double>> ORCA::to_latlon() const {
    return {latitudes_, longitudes_};
}


static const GridRegisterType<ORCA> __grid_type("ORCA");
static const GridRegisterName<ORCA> __grid_pattern(GridRegisterName<ORCA>::uid_pattern);


}  // namespace eckit::geometry::grid
