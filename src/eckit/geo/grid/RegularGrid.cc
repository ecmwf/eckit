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


#include "eckit/geo/grid/RegularGrid.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <ostream>
#include <sstream>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/config/Resource.h"
#include "eckit/geo/Earth.h"
#include "eckit/geo/Iterator.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::grid {


RegularGrid::RegularGrid(Projection* projection, const Configuration& config) :
    Grid(config),
    projection_(projection),
    figure_(config),
    xPlus_(true),
    yPlus_(false) {
    ASSERT(projection_);

    auto get_long_first_key = [](const Configuration& config, const std::vector<std::string>& keys) -> long {
        long value = 0;
        for (const auto& key : keys) {
            if (config.get(key, value)) {
                return value;
            }
        }
        throw SeriousBug("RegularGrid: couldn't find any key: " + StringTools::join(", ", keys));
    };

    long nx = get_long_first_key(config, {"numberOfPointsAlongXAxis", "Ni"});
    long ny = get_long_first_key(config, {"numberOfPointsAlongYAxis", "Nj"});
    ASSERT(nx > 0);
    ASSERT(ny > 0);

    auto grid = config.getDoubleVector("grid");
    ASSERT(grid.size() == 2);

    auto first    = projection->fwd(PointLonLat{config.getDouble("longitudeOfFirstGridPointInDegrees"), config.getDouble("latitudeOfFirstGridPointInDegrees")});
    auto first_xy = std::get<Point2>(first);

    config.get("iScansPositively", xPlus_);  // iScansPositively != 0
    config.get("jScansPositively", yPlus_);  // jScansPositively == 0

    auto firstPointBottomLeft_ = config.getBool("first_point_bottom_left", false);

    auto linspace = [](double start, double step, long num, bool plus) -> LinearSpacing {
        ASSERT(step >= 0.);
        //        return {start, start + step * double(plus ? num - 1 : 1 - num), num};
        NOTIMP;
    };

    x_ = linspace(first_xy.X, grid[0], nx, firstPointBottomLeft_ || xPlus_);
    y_ = linspace(first_xy.Y, grid[1], ny, firstPointBottomLeft_ || yPlus_);

    ASSERT(!x_.empty());
    ASSERT(!y_.empty());
    auto n = y_.front() > y_.back() ? y_.front() : y_.back();
    auto s = y_.front() > y_.back() ? y_.back() : y_.front();
    auto w = x_.front() > x_.back() ? x_.back() : x_.front();
    auto e = x_.front() > x_.back() ? x_.front() : x_.back();

    bbox(BoundingBox::make({n, w, s, e}, *projection_));
}


RegularGrid::RegularGrid(Projection* projection, const BoundingBox& bbox, const LinearSpacing& x,
                         const LinearSpacing& y, const Figure& figure) :
    Grid(bbox),
    x_(x),
    y_(y),
    figure_(figure),
    xPlus_(x.front() <= x.back()),
    yPlus_(y.front() < y.back()) {
}


RegularGrid::~RegularGrid() = default;


Projection* RegularGrid::make_projection_from_proj(const Configuration& config) {
    if (std::string proj;
        config.get("proj", proj)) {
        return ProjectionFactory::build("proj", config);
    }

    return nullptr;
}


void RegularGrid::print(std::ostream& out) const {
    NOTIMP;
}


size_t RegularGrid::numberOfPoints() const {
    return x_.size() * y_.size();
}


bool RegularGrid::isPeriodicWestEast() const {
    return false;
}


bool RegularGrid::includesNorthPole() const {
    return bbox().north() == NORTH_POLE;
}


bool RegularGrid::includesSouthPole() const {
    return bbox().south() == SOUTH_POLE;
}


Renumber RegularGrid::crop(BoundingBox& bbox) const {
    NOTIMP;
}


Renumber RegularGrid::reorder(long /*scanningMode*/) const {
    NOTIMP;
}


Iterator* RegularGrid::iterator() const {
    class RegularGridIterator : public Iterator {
        const Projection& projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;

        size_t ni_;
        size_t nj_;
        size_t i_;
        size_t j_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "RegularGridIterator[";
            Iterator::print(out);
            out << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool operator++() override {
            if (j_ < nj_ && i_ < ni_) {
                const auto p  = projection_.inv(Point2{x_[i_], y_[j_]});
                const auto ll = std::get<PointLonLat>(p);
                //                _lat     = ll.lat;
                //                _lon     = ll.lon;

                if (i_ > 0 || j_ > 0) {
                    count_++;
                }

                if (++i_ == ni_) {
                    i_ = 0;
                    j_++;
                }

                return true;
            }
            return false;
        }

        size_t index() const override { return count_; }

        size_t size() const override { NOTIMP; }

    public:
        RegularGridIterator(const Projection& projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(projection), x_(x), y_(y), ni_(x.size()), nj_(y.size()), i_(0), j_(0), count_(0) {}
        ~RegularGridIterator() override = default;

        RegularGridIterator(const RegularGridIterator&)            = delete;
        RegularGridIterator(RegularGridIterator&&)                 = delete;
        RegularGridIterator& operator=(const RegularGridIterator&) = delete;
        RegularGridIterator& operator=(RegularGridIterator&&)      = delete;
    };

    return new RegularGridIterator(*projection_, x_, y_);
}


struct Lambert : RegularGrid {
    Lambert(const Configuration& config) :
        RegularGrid(make_projection(config), config) {
    }

    static Projection* make_projection(const Configuration& config) {
        if (auto p = RegularGrid::make_projection_from_proj(config); p != nullptr) {
            return p;
        }

        auto LaDInDegrees    = config.getDouble("LaDInDegrees");
        auto LoVInDegrees    = config.getDouble("LoVInDegrees");
        auto Latin1InDegrees = config.getDouble("Latin1InDegrees", LaDInDegrees);
        auto Latin2InDegrees = config.getDouble("Latin2InDegrees", LaDInDegrees);

        return ProjectionFactory::build("lambert_conformal_conic",
                                        MappedConfiguration({{"latitude1", Latin1InDegrees},
                                                             {"latitude2", Latin2InDegrees},
                                                             {"latitude0", LaDInDegrees},
                                                             {"longitude0", LoVInDegrees}}));
    }
};


struct LambertAzimuthalEqualArea : RegularGrid {

    LambertAzimuthalEqualArea(const Configuration& config) :
        RegularGrid(make_projection(config), config) {}

    LambertAzimuthalEqualArea(Projection* projection, const BoundingBox& bbox, const LinearSpacing& x, const LinearSpacing& y, const Figure& shape) :
        RegularGrid(projection, bbox, x, y, shape) {}

    static Projection* make_projection(const Configuration& config) {
        if (auto p = RegularGrid::make_projection_from_proj(config); p != nullptr) {
            return p;
        }

        double standardParallel = 0.;
        double centralLongitude = 0.;
        double radius           = 0.;
        ASSERT(config.get("standardParallelInDegrees", standardParallel));
        ASSERT(config.get("centralLongitudeInDegrees", centralLongitude));
        config.get("radius", radius = Earth::radius());

        return ProjectionFactory::build("lambert_azimuthal_equal_area", MappedConfiguration({{"standard_parallel", standardParallel},
                                                                                             {"central_longitude", centralLongitude},
                                                                                             {"radius", radius}}));
    }
};


struct Mercator : RegularGrid {
    Mercator(const Configuration& config) :
        RegularGrid(RegularGrid::make_projection_from_proj(config), config) {}
};


struct PolarStereographic : RegularGrid {
    PolarStereographic(const Configuration& config) :
        RegularGrid(RegularGrid::make_projection_from_proj(config), config) {}
};


static const GridBuilder<Lambert> __builder1("lambert");
static const GridBuilder<LambertAzimuthalEqualArea> __builder2("lambert_azimuthal_equal_area");
static const GridBuilder<Mercator> __builder3("mercator");
static const GridBuilder<PolarStereographic> __builder4("polar_stereographic");


}  // namespace eckit::geo::grid
