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


#include "eckit/geo/grid/HEALPix.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <regex>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/container/PointsContainer.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid {


static const std::string HEALPIX_PATTERN_1 = "[hH]([1-9][0-9]*)(n|_nested|r|_ring)?";
// static const std::string HEALPIX_PATTERN_2 = "[hH][rRnN]([1-9][0-9]*)";


HEALPix::HEALPix(const Spec& spec) :
    HEALPix(util::convert_long_to_size_t(spec.get_long("Nside")), spec.get_string("order", "ring")) {}


HEALPix::HEALPix(size_t Nside, order_type order) : Nside_(Nside), order_(order, static_cast<int>(Nside)) {}


Grid::iterator HEALPix::cbegin() const {
    return order() == order::HEALPix::ring ? iterator{new geo::iterator::Reduced(*this, 0)}
                                           : iterator{new geo::iterator::Unstructured(
                                                 *this, 0, std::make_shared<container::PointsInstance>(to_points()))};
}


Grid::iterator HEALPix::cend() const {
    return order() == order::HEALPix::ring ? iterator{new geo::iterator::Reduced(*this, size())}
                                           : iterator{new geo::iterator::Unstructured(*this)};
}


size_t HEALPix::ni(size_t j) const {
    ASSERT(j < nj());
    return j < Nside_ ? 4 * (j + 1) : j < 3 * Nside_ ? 4 * Nside_ : ni(nj() - 1 - j);
}


size_t HEALPix::nj() const {
    return 4 * Nside_ - 1;
}


Spec* HEALPix::spec(const std::string& name) {
    static const std::regex rex(HEALPIX_PATTERN_1);

    std::smatch match;
    ASSERT(std::regex_match(name, match, rex));

    const auto end   = match[2].str();
    const auto order = end == "n" || end == ("_" + order::HEALPix::nested) ? order::HEALPix::nested
                       : end.empty() || end == "r" || end == ("_" + order::HEALPix::ring)
                           ? order::HEALPix::ring
                           : throw exception::OrderError("HEALPix: supported ordering: ring, nested", Here());

    return new spec::Custom{{"type", "HEALPix"}, {"Nside", std::stoul(match[1])}, {"order", order}};
}


size_t HEALPix::size() const {
    return 12 * Nside_ * Nside_;
}


std::vector<Point> HEALPix::to_points() const {
    const auto points = Reduced::to_points();

    if (order() == order::HEALPix::ring) {
        return points;
    }

    ASSERT(order() == order::HEALPix::nested);

    std::vector<Point> points_nested;
    points_nested.reserve(size());

    for (size_t i = 0; i < size(); ++i) {
        points_nested.emplace_back(std::get<PointLonLat>(points[order_.nest_to_ring(static_cast<int>(i))]));
    }

    return points_nested;
}


std::pair<std::vector<double>, std::vector<double>> HEALPix::to_latlons() const {
    std::pair<std::vector<double>, std::vector<double>> latlon;
    latlon.first.reserve(size());
    latlon.second.reserve(size());

    for (const auto& p : to_points()) {
        const auto& q = std::get<PointLonLat>(p);
        latlon.first.push_back(q.lat);
        latlon.second.push_back(q.lon);
    }

    return latlon;
}


const std::vector<double>& HEALPix::latitudes() const {
    const auto Nj = nj();

    if (latitudes_.empty()) {
        latitudes_.resize(Nj);

        auto i = latitudes_.begin();
        auto j = latitudes_.rbegin();
        for (size_t ring = 1; ring < 2 * Nside_; ++ring, ++i, ++j) {
            const auto f = ring < Nside_
                               ? 1. - static_cast<double>(ring * ring) / (3 * static_cast<double>(Nside_ * Nside_))
                               : 4. / 3. - 2 * static_cast<double>(ring) / (3 * static_cast<double>(Nside_));

            *i = 90. - util::RADIAN_TO_DEGREE * std::acos(f);
            *j = -*i;
        }
        *i = 0.;
    }

    ASSERT(latitudes_.size() == Nj);
    return latitudes_;
}


std::vector<double> HEALPix::longitudes(size_t j) const {
    const auto Ni    = ni(j);
    const auto step  = 360. / static_cast<double>(Ni);
    const auto start = j < Nside_ || 3 * Nside_ - 1 < j || static_cast<bool>((j + Nside_) % 2) ? step / 2. : 0.;

    std::vector<double> lons(Ni);
    std::generate_n(lons.begin(), Ni,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return lons;
}


void HEALPix::fill_spec(spec::Custom& custom) const {
    custom.set("grid", "H" + std::to_string(Nside_));

    if (order() != order::HEALPix::order_default()) {
        custom.set("order", order());
    }
}


const std::string& HEALPix::type() const {
    static const std::string type{"healpix"};
    return type;
}


static const GridRegisterType<HEALPix> GRIDTYPE1("HEALPix");
static const GridRegisterType<HEALPix> GRIDTYPE2("healpix");
static const GridRegisterName<HEALPix> GRIDNAME(HEALPIX_PATTERN_1);


}  // namespace eckit::geo::grid
