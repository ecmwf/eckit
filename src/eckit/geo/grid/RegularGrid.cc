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
#include "eckit/geo/Iterator.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::grid {


RegularGrid::RegularGrid(const Configuration& config, Projection* projection) :Grid(config),
    projection_(projection),
    shape_(config), xPlus_(true), yPlus_(false), firstPointBottomLeft_(false) {
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

    std::vector<double> grid;
    ASSERT(config.get("grid", grid));
    ASSERT(grid.size() == 2);

    Point2 firstLL;
    ASSERT(config.get("latitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LAT]));
    ASSERT(config.get("longitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LON]));
    auto first = projection.xy(firstLL);

    config.get("iScansPositively", xPlus_);  // iScansPositively != 0
    config.get("jScansPositively", yPlus_);  // jScansPositively == 0
    config.get("first_point_bottom_left", firstPointBottomLeft_);

    x_    = linspace(first.x(), grid[0], nx, firstPointBottomLeft_ || xPlus_);
    y_    = linspace(first.y(), grid[1], ny, firstPointBottomLeft_ || yPlus_);
    grid_ = {x_, y_, projection};

#if 0
    atlas::RectangularDomain range({x_.min(), x_.max()}, {y_.min(), y_.max()}, "meters");

    auto box = projection.lonlatBoundingBox(range);
    ASSERT(box);

    bbox({box.north(), box.west(), box.south(), box.east()});
#else
    bbox({y_.max(), x_.min(), y_.min(), x_.max()});
#endif
}


RegularGrid::RegularGrid(const Projection& projection, const BoundingBox& bbox, const LinearSpacing& x,
                         const LinearSpacing& y, const Shape& shape) :
    Grid(bbox),
    x_(x),
    y_(y),
    shape_(shape),
    xPlus_(x.front() <= x.back()),
    yPlus_(y.front() < y.back()),
    firstPointBottomLeft_(false) {
    grid_ = {x_, y_, projection};

    if (!shape_.provided) {
        shape_ = {grid_.projection().spec()};
    }
}


RegularGrid::~RegularGrid() = default;


RegularGrid::Configuration RegularGrid::make_projection_from_proj(const Configuration& config) {
    std::string proj;
    config.get("proj", proj);

    if (proj.empty() || !::atlas::projection::ProjectionFactory::has("proj")) {
        return {};
    }

    Configuration spec("type", "proj");
    spec.set("proj", proj);

    std::string projSource;
    if (config.get("projSource", projSource) && !projSource.empty()) {
        spec.set("proj_source", projSource);
    }

    std::string projGeocentric;
    if (config.get("projGeocentric", projGeocentric) && !projGeocentric.empty()) {
        spec.set("proj_geocentric", projGeocentric);
    }

    return spec;
}


RegularGrid::LinearSpacing RegularGrid::linspace(double start, double step, long num, bool plus) {
    ASSERT(step >= 0.);
    return {start, start + step * double(plus ? num - 1 : 1 - num), num};
}


void RegularGrid::print(std::ostream& out) const {
    out << "RegularGrid[x=" << x_.spec() << ",y=" << y_.spec() << ",projection=" << grid_.projection().spec()
        << ",firstPointBottomLeft=" << firstPointBottomLeft_ << ",bbox=" << bbox_ << "]";
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
        Projection* projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;
        PointLonLat pLonLat_;

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

        bool next(double& _lat, double& _lon) override {
            if (j_ < nj_ && i_ < ni_) {
                pLonLat_ = projection_.lonlat({x_[i_], y_[j_]});
                _lat     = lat(pLonLat_.lat());
                _lon     = lon(pLonLat_.lon());

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
        RegularGridIterator(Projection projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(std::move(projection)), x_(x), y_(y), ni_(x.size()), nj_(y.size()), i_(0), j_(0), count_(0) {}
        ~RegularGridIterator() override = default;

        RegularGridIterator(const RegularGridIterator&)            = delete;
        RegularGridIterator(RegularGridIterator&&)                 = delete;
        RegularGridIterator& operator=(const RegularGridIterator&) = delete;
        RegularGridIterator& operator=(RegularGridIterator&&)      = delete;
    };

    return new RegularGridIterator(grid_.projection(), x_, y_);
}


struct Lambert :  RegularGrid {
    Lambert(const Configuration& config) :
        RegularGrid(config, make_projection(config)) {
    }

    static Projection* make_projection(const Configuration& config) {
        if (auto spec = RegularGrid::make_projection_from_proj(config); spec != nullptr) {
            return spec;
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


struct LambertAzimuthalEqualArea :  RegularGrid {

    LambertAzimuthalEqualArea(const Configuration& config) :
        RegularGrid(config, make_projection(config)) {}

    LambertAzimuthalEqualArea(const Projection& projection, const BoundingBox& bbox, const LinearSpacing& x, const LinearSpacing& y, const Shape& shape) :
        RegularGrid(projection, bbox, x, y, shape) {}

    static Projection* make_projection(const Configuration& config) {
        if (auto spec = RegularGrid::make_projection_from_proj(config); spec != nullptr) {
            return spec;
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


struct Mercator :  RegularGrid {
   Mercator(const Configuration&) :
        RegularGrid(config, RegularGrid::make_projection_from_proj(config)) {}
};


struct PolarStereographic :  RegularGrid {
    PolarStereographic(const Configuration& config) :
        RegularGrid(config, RegularGrid::make_projection_from_proj(config)) {}
};


static const GridBuilder<Lambert> __builder("lambert");
static const GridBuilder<LambertAzimuthalEqualArea> __builder("lambert_azimuthal_equal_area");
static const GridBuilder<Mercator> __builder("mercator");
static const GridBuilder<PolarStereographic> __builder("polar_stereographic");


}  // namespace eckit::geo::grid
