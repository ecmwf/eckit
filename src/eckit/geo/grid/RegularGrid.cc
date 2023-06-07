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

#include "eckit/config/Resource.h"
#include "eckit/geo/Iterator.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::grid {


RegularGrid::RegularGrid(const Configuration& param, const Projection& projection) :
    shape_(param), xPlus_(true), yPlus_(false), firstPointBottomLeft_(false) {
    ASSERT(projection);

    auto get_long_first_key = [](const Configuration& param, const std::vector<std::string>& keys) -> long {
        long value = 0;
        for (const auto& key : keys) {
            if (param.get(key, value)) {
                return value;
            }
        }
        throw SeriousBug("RegularGrid: couldn't find any key: " + StringTools::join(", ", keys));
    };

    long nx = get_long_first_key(param, {"numberOfPointsAlongXAxis", "Ni"});
    long ny = get_long_first_key(param, {"numberOfPointsAlongYAxis", "Nj"});
    ASSERT(nx > 0);
    ASSERT(ny > 0);

    std::vector<double> grid;
    ASSERT(param.get("grid", grid));
    ASSERT_KEYWORD_GRID_SIZE(grid.size());

    Point2 firstLL;
    ASSERT(param.get("latitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LAT]));
    ASSERT(param.get("longitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LON]));
    auto first = projection.xy(firstLL);

    param.get("iScansPositively", xPlus_);  // iScansPositively != 0
    param.get("jScansPositively", yPlus_);  // jScansPositively == 0
    param.get("first_point_bottom_left", firstPointBottomLeft_);

    x_    = linspace(first.x(), grid[0], nx, firstPointBottomLeft_ || xPlus_);
    y_    = linspace(first.y(), grid[1], ny, firstPointBottomLeft_ || yPlus_);
    grid_ = {x_, y_, projection};

    atlas::RectangularDomain range({x_.min(), x_.max()}, {y_.min(), y_.max()}, "meters");

    auto box = projection.lonlatBoundingBox(range);
    ASSERT(box);

    bbox({box.north(), box.west(), box.south(), box.east()});
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


RegularGrid::Configuration RegularGrid::make_proj_spec(const Configuration& param) {
    std::string proj;
    param.get("proj", proj);

    if (proj.empty() || !::atlas::projection::ProjectionFactory::has("proj")) {
        return {};
    }

    Configuration spec("type", "proj");
    spec.set("proj", proj);

    std::string projSource;
    if (param.get("projSource", projSource) && !projSource.empty()) {
        spec.set("proj_source", projSource);
    }

    std::string projGeocentric;
    if (param.get("projGeocentric", projGeocentric) && !projGeocentric.empty()) {
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


bool RegularGrid::crop(BoundingBox& bbox, Renumber& mapping) const {
    auto mm = minmax_ij(bbox);
    auto Ni = x_.size();
    auto N  = (mm.second.i - mm.first.i + 1) * (mm.second.j - mm.first.j + 1);
    mapping.clear();
    mapping.reserve(N);

    for (std::unique_ptr<Iterator> it(iterator()); it->next();) {
        auto i = it->index() % Ni;
        auto j = it->index() / Ni;
        if (mm.first.i <= i && i <= mm.second.i && mm.first.j <= j && j <= mm.second.j) {
            mapping.push_back(it->index());
        }
    }
    ASSERT(mapping.size() == N);

    return true;
}


size_t RegularGrid::numberOfPoints() const {
    return x_.size() * y_.size();
}


bool RegularGrid::isPeriodicWestEast() const {
    return false;
}


bool RegularGrid::includesNorthPole() const {
    return bbox_.north() == NORTH_POLE;
}


bool RegularGrid::includesSouthPole() const {
    return bbox_.south() == SOUTH_POLE;
}


void RegularGrid::reorder(long /*scanningMode*/) const {
    // do not reorder, iterator is doing the right thing
    // FIXME this function should not be overriding to do nothing
}


Iterator* RegularGrid::iterator() const {
    class RegularGridIterator : public Iterator {
        Projection projection_;
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


}  // namespace eckit::geo::grid
