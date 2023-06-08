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


#include "eckit/geo/grid/SpaceView.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geo/Iterator.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::grid {


static const GridBuilder<SpaceView> __builder("space_view");


namespace detail {


template <typename EXTERNAL_T, typename INTERNAL_T = EXTERNAL_T>
EXTERNAL_T get(const Configuration& config, const std::string& key) {
    INTERNAL_T value;
    ASSERT(config.get(key, value));
    return static_cast<EXTERNAL_T>(value);
}


SpaceViewInternal::SpaceViewInternal(const Configuration& config) {
    auto earthIsOblate = get<bool>(config, "earthIsOblate");
    auto a             = get<double>(config, earthIsOblate ? "earthMajorAxis" : "radius");
    auto b             = earthIsOblate ? get<double>(config, "earthMinorAxis") : a;

    auto Nr = get<double>(config, "NrInRadiusOfEarthScaled");
    ASSERT(Nr > 1.);

    auto h = (Nr - 1.) * a;

    Lop_     = get<double>(config, "longitudeOfSubSatellitePointInDegrees");
    auto Lap = get<double>(config, "latitudeOfSubSatellitePointInDegrees");
    ASSERT(types::is_approximately_equal(Lap, 0.));

    // ASSERT(get<size_t>(config, "orientationOfTheGridInDegrees") == 180);


    // projection
    auto proj = [](double h, double a, double b, double lon_0) {
        auto _h = " +h=" + std::to_string(h);
        auto _l = types::is_approximately_equal(lon_0, 0.) ? "" : " +lon_0=" + std::to_string(lon_0);
        auto _e = types::is_approximately_equal(a, b) ? " +R=" + std::to_string(a)
                                                      : " +a=" + std::to_string(a) + " +b=" + std::to_string(b);
        return "+proj=geos +type=crs +sweep=y" + _h + _l + _e;
    };

    projection_.reset(ProjectionFactory::build("proj", MappedConfiguration({{"proj", proj(h, a, b, Lop_)}})));

    projectionGreenwich_.reset(ProjectionFactory::build("proj", MappedConfiguration({{"proj", proj(h, a, b, 0)}})));

    // (x, y) space
    Nx_ = get<long>(config, "Nx");
    ASSERT(1 < Nx_);

    auto ip = get<bool>(config, "iScansPositively");
    auto xp = get<double, long>(config, "XpInGridLengths");
    auto dx = get<double, long>(config, "dx");
    ASSERT(dx > 0);

    auto rx = 2. * std::asin(1. / Nr) / dx * h;  // (height factor is PROJ-specific)

    (ip ? xa_ : xb_) = rx * (-xp);
    (ip ? xb_ : xa_) = rx * (-xp + double(Nx_ - 1));

    Ny_ = get<long>(config, "Ny");
    ASSERT(1 < Ny_);

    auto jp = get<bool>(config, "jScansPositively");
    auto yp = get<double, long>(config, "YpInGridLengths");
    auto dy = get<double, long>(config, "dy");
    ASSERT(dy > 0);

    auto ry = 2. * std::asin(1. / Nr) / dy * h;  // (height factor is PROJ-specific)

    (jp ? ya_ : yb_) = ry * (-yp);
    (jp ? yb_ : ya_) = ry * (-yp + double(Ny_ - 1));


    // longest element diagonal, a multiple of a reference central element diagonal (avoiding distortion)
    LongestElementDiagonal_ = 20. * Earth::distance(projection_->inv(geometry::Point2{-rx / 2, ry / 2}), projection_->inv(geometry::Point2{rx / 2, -ry / 2}));
    ASSERT(0. < LongestElementDiagonal_);


    // bounding box
#if 1
    // [1] page 25, solution of s_d^2=0, restrained at x=0 (lon) and y=0 (lat). Note: uses a, b, height defined there
    auto eps_ll = 1e-6;

    auto n = 90. - util::radian_to_degree * 0.151347 + eps_ll;
    auto s = -n;

    auto e = 90. - util::radian_to_degree * 0.151853 + eps_ll + Lop_;
    auto w = 2. * Lop_ - e;
#else
    auto geometric_maximum = [](double x_min, double x_eps, const std::function<double(double)>& f,
                                double f_eps = 1.e-9, size_t it_max = 1000) {
        size_t it = 0;
        auto x    = x_min;
        auto fx   = f(x);

        for (auto dx = x_eps; f_eps < dx && it < it_max; ++it) {
            auto fx_new = f(x + dx);
            if (!std::isfinite(fx_new) || fx_new < fx) {
                dx /= 2.;
            }
            else {
                x += dx;
                fx = fx_new;
                dx *= 2.;
            }
        }

        ASSERT(0. < fx && fx < 90.);
        return fx;
    };

    auto eps_xy = 1e-6 * h;
    auto eps_ll = 1e-6;

    auto max_lon = geometric_maximum(0., eps_xy, [&](double x) { return projectionGreenwich_->inv({x, 0}).lon(); });
    auto w       = Lop_ - max_lon - eps_ll;
    auto e       = Lop_ + max_lon + eps_ll;

    auto max_lat = geometric_maximum(0., eps_xy, [&](double y) { return projectionGreenwich_->inv({0, y}).lat(); });
    auto n       = max_lat + eps_ll;
    auto s       = -n;
#endif

    bbox_ = {n, w, s, e};
}


const std::vector<PointLonLat>& SpaceViewInternal::lonlat() const {
    if (lonlat_.empty()) {
        ASSERT(projectionGreenwich_);  // Greenwich-centred (avoids PROJ normalisation)
        lonlat_.reserve(Nx_ * Ny_);

        size_t index = 0;
        for (const auto& _y : y()) {
            for (const auto& _x : x()) {
                //                auto& ll = lonlat_[index++];
                PointLonLat ll = projectionGreenwich_->inv({_x, _y});
                if (std::isfinite(ll.lon) && std::isfinite(ll.lat)) {
                    ASSERT(-90. < ll.lon && ll.lon < 90.);

                    ll.lon += Lop_;
                    lonlat_.push_back(ll);
                }
            }
        }
    }

    ASSERT(!lonlat_.empty());
    return lonlat_;
}


}  // namespace detail


SpaceView::SpaceView(const Configuration& config) :
    detail::SpaceViewInternal(config),
    RegularGrid(SpaceViewInternal::projection_, SpaceViewInternal::bbox_, SpaceViewInternal::x(),
                SpaceViewInternal::y(), {config}) {}


Iterator* SpaceView::iterator() const {
    class SpaceViewIterator : public Iterator {
        const std::vector<PointLonLat>& lonlat_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "SpaceViewIterator[";
            Iterator::print(out);
            out << ",count=" << count_ << "]";
        }

        bool next(double& _lat, double& _lon) override {
            while (count_ < lonlat_.size()) {
                // only one of (lon, lat) needs to be checked
                const auto& ll = lonlat_[count_++];
                if (std::isfinite(ll.lon())) {
                    _lat = lat(ll.lat());
                    _lon = lon(ll.lon());
                    return true;
                }
            }

            return false;
        }

        size_t index() const override { return count_; }

        size_t size() const override { NOTIMP; }

    public:
        SpaceViewIterator(const std::vector<PointLonLat>& lonlat) :
            lonlat_(lonlat), count_(0) {}
        ~SpaceViewIterator() override = default;

        SpaceViewIterator(const SpaceViewIterator&)            = delete;
        SpaceViewIterator(SpaceViewIterator&&)                 = delete;
        SpaceViewIterator& operator=(const SpaceViewIterator&) = delete;
        SpaceViewIterator& operator=(SpaceViewIterator&&)      = delete;
    };

    return new SpaceViewIterator(lonlat());
}


}  // namespace eckit::geo::grid
