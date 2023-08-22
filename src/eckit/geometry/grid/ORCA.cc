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

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/area/BoundingBox.h"


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


size_t dimension(const std::vector<size_t>& dim, size_t index) {
    ASSERT(dim.size() == 2 && index < 2);
    return dim[index];
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
    ni_(dimension(config.getUnsignedVector("dimensions"), 0)),
    nj_(dimension(config.getUnsignedVector("dimensions"), 1)),
    name_(config.getString("orca_name")),
    uid_(config.getString("orca_uid")),
    arrangement_(arrangement_from_string(config.getString("orca_arrangement"))) {
    ASSERT(0 < ni_);
    ASSERT(0 < nj_);

    auto url = config.getString("url_prefix", "") + config.getString("url");
    Log::info() << "url: '" << url << "'" << std::endl;
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
