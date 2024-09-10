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


#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::util {


PointLonLat longitude_in_range(double reference, const PointLonLat& p) {
    // keep longitude difference (to reference) range below +-180 degree
    auto lon = p.lon;
    while (lon > reference + 180.) {
        lon -= 360.;
    }
    while (lon <= reference - 180.) {
        lon += 360.;
    }
    return {lon, p.lat};
}


struct BoundLonLat {
    BoundLonLat(PointLonLat min, PointLonLat max) : min_(min), max_(max) {}

    explicit operator area::BoundingBox() const { return {max_.lat, min_.lon, min_.lat, max_.lon}; }

    void extend(PointLonLat p, PointLonLat eps) {
        ASSERT(0. <= eps.lon && 0. <= eps.lat);

        auto sub = p - eps;
        auto add = p + eps;
        min_     = first_ ? sub : PointLonLat::componentsMin(min_, sub);
        max_     = first_ ? add : PointLonLat::componentsMax(max_, add);
        first_   = false;

        min_ = {min_.lon, std::max(min_.lat, -90.)};
        max_ = {std::min(max_.lon, min_.lon + 360.), std::min(max_.lat, 90.)};
        ASSERT(min_.lon <= max_.lon && min_.lat <= max_.lat);

        includesSouthPole(types::is_approximately_equal(min_.lat, -90.));
        includesNorthPole(types::is_approximately_equal(max_.lat, 90.));
        crossesDateLine(types::is_approximately_equal(max_.lon - min_.lon, 360.));
    }

    bool crossesDateLine(bool yes) {
        if ((crossesDateLine_ = crossesDateLine_ || yes)) {
            max_ = {min_.lon + 360., max_.lat};
        }
        return crossesDateLine_;
    }

    bool includesNorthPole(bool yes) {
        if ((includesNorthPole_ = includesNorthPole_ || yes)) {
            max_ = {max_.lon, 90.};
        }
        crossesDateLine(includesNorthPole_);
        return includesNorthPole_;
    }

    bool includesSouthPole(bool yes) {
        if ((includesSouthPole_ = includesSouthPole_ || yes)) {
            min_ = {min_.lon, -90.};
        }
        crossesDateLine(includesSouthPole_);
        return includesSouthPole_;
    }

    bool crossesDateLine() const { return crossesDateLine_; }
    bool includesNorthPole() const { return includesNorthPole_; }
    bool includesSouthPole() const { return includesSouthPole_; }

private:
    PointLonLat min_;
    PointLonLat max_;
    bool crossesDateLine_   = false;
    bool includesNorthPole_ = false;
    bool includesSouthPole_ = false;
    bool first_             = true;
};


struct Derivate {
    Derivate(const Projection& p, Point2 A, Point2 B, double h, double refLongitude = 0.) :
        projection_(p), H_{Point2::normalize(B - A) * h}, invnH_(1. / Point2::norm(H_)), refLongitude_(refLongitude) {}

    virtual ~Derivate() = default;

    Derivate(const Derivate&)       = delete;
    Derivate(Derivate&&)            = delete;
    void operator=(const Derivate&) = delete;
    void operator=(Derivate&&)      = delete;

    virtual PointLonLat d(Point2) const = 0;

    PointLonLat f(const Point2& p) const {
        return longitude_in_range(refLongitude_, std::get<PointLonLat>(projection_.inv(p)));
    }

    inline const Point2& H() const { return H_; }
    inline double invnH() const { return invnH_; }

private:
    const Projection& projection_;
    const Point2 H_;
    const double invnH_;
    const double refLongitude_;
};


struct DerivateForwards final : Derivate {
    using Derivate::Derivate;
    PointLonLat d(Point2 P) const override { return (f(P + H()) - f(P)) * invnH(); }
};


struct DerivateBackwards final : Derivate {
    using Derivate::Derivate;
    PointLonLat d(Point2 P) const override { return (f(P) - f(P - H())) * invnH(); }
};


struct DerivateCentral final : Derivate {
    DerivateCentral(const Projection& p, Point2 A, Point2 B, double h, double refLongitude) :
        Derivate(p, A, B, h, refLongitude), H2_{H() * 0.5} {}
    const Point2 H2_;
    PointLonLat d(Point2 P) const override { return (f(P + H2_) - f(P - H2_)) * invnH(); }
};


struct DerivateFactory {
    static const Derivate* build(const std::string& type, const Projection& p, Point2 A, Point2 B, double h,
                                 double refLongitude = 0.) {
        ASSERT(0. < h);

        if (A.distance2(B) < h * h) {
            struct DerivateDegenerate final : Derivate {
                using Derivate::Derivate;
                PointLonLat d(Point2) const override { return {99, 99}; }  // FIXME
            };
            return new DerivateDegenerate(p, A, B, h, refLongitude);
        }

        return type == "forwards"    ? static_cast<Derivate*>(new DerivateForwards(p, A, B, h, refLongitude))
               : type == "backwards" ? static_cast<Derivate*>(new DerivateBackwards(p, A, B, h, refLongitude))
               : type == "central"   ? static_cast<Derivate*>(new DerivateCentral(p, A, B, h, refLongitude))
                                     : throw BadValue("DerivateFactory: unknown method", Here());
    }

    static void list(std::ostream& out) { return instance().list_(out); }

private:
    static DerivateFactory& instance() {
        static DerivateFactory obj;
        return obj;
    }

    // This is 'const' as Grid should always be immutable
    const Derivate* build_(const std::string& type, const Projection& p, Point2 A, Point2 B, double h,
                           double refLongitude) const;

    void list_(std::ostream&) const;
};


area::BoundingBox bounding_box(Point2 min, Point2 max, Projection& projection) {
    using types::is_strictly_greater;


    // 0. setup

    // use central longitude as absolute reference (keep points within +-180 longitude range)
    const Point2 centre_xy{(min.X + max.X) / 2., (min.Y + max.Y) / 2.};
    const auto centre_ll  = std::get<PointLonLat>(projection.inv(centre_xy));  // asserts fwd(PointLonLat) -> Point2
    const auto centre_lon = centre_ll.lon;

    const std::string derivative_type = "central";
    constexpr double h_ll             = 0.5e-6;  // precision to microdegrees
    constexpr double h                = 0.5e-1;  // precision to decimeters
    constexpr size_t Niter            = 100;


    // 1. determine box from projected corners

    struct : public std::pair<Point2, Point2> {
        using pair::pair;
        bool contains(const Point2& P) const {
            return (first.X < P.X && P.X < second.X) && (first.Y < P.Y && P.Y < second.Y);
        }
    } rect(min, max);

    const std::pair<Point2, Point2> segments[] = {{{min.X, max.Y}, {max.X, max.Y}},
                                                  {{max.X, max.Y}, {max.X, min.Y}},
                                                  {{max.X, min.Y}, {min.X, min.Y}},
                                                  {{min.X, min.Y}, {min.X, max.Y}}};

    BoundLonLat bounds(centre_ll, centre_ll);
    for (const auto& [A, dummy] : segments) {
        auto q = longitude_in_range(centre_lon, std::get<PointLonLat>(projection.inv(A)));
        bounds.extend(q, PointLonLat{h_ll, h_ll});
    }


    // 2. locate latitude extrema by checking if poles are included (in the un-projected frame) and if not, find extrema
    // not at the corners by refining iteratively

    if (!bounds.includesNorthPole()) {
        bounds.includesNorthPole(rect.contains(std::get<Point2>(projection.fwd(PointLonLat{0., 90. - h_ll}))));
    }

    if (!bounds.includesSouthPole()) {
        bounds.includesSouthPole(rect.contains(std::get<Point2>(projection.fwd(PointLonLat{0., -90. + h_ll}))));
    }

    for (auto [A, B] : segments) {
        if (!bounds.includesNorthPole() || !bounds.includesSouthPole()) {
            std::unique_ptr<const Derivate> derivate(
                DerivateFactory::build(derivative_type, projection, A, B, h, centre_lon));

            double dAdy = derivate->d(A).lat;
            double dBdy = derivate->d(B).lat;

            if (!is_strictly_greater(dAdy * dBdy, 0.)) {
                PointLonLat H{0, h_ll};

                for (size_t cnt = 0; cnt < Niter; ++cnt) {
                    Point2 M    = Point2::middle(A, B);
                    double dMdy = derivate->d(M).lat;
                    if (is_strictly_greater(dAdy * dMdy, 0.)) {
                        A    = M;
                        dAdy = dMdy;
                    }
                    else if (is_strictly_greater(dBdy * dMdy, 0.)) {
                        B    = M;
                        dBdy = dMdy;
                    }
                    else {
                        break;
                    }
                }

                // update extrema, extended by 'a small amount' (arbitrary)
                bounds.extend(std::get<PointLonLat>(projection.inv(Point2::middle(A, B))), H);
            }
        }
    }


    // 3. locate longitude extrema not at the corners by refining iteratively

    for (auto [A, B] : segments) {
        if (!bounds.crossesDateLine()) {
            std::unique_ptr<const Derivate> derivate(
                DerivateFactory::build(derivative_type, projection, A, B, h, centre_lon));

            double dAdx = derivate->d(A).lon;
            double dBdx = derivate->d(B).lon;

            if (!is_strictly_greater(dAdx * dBdx, 0.)) {
                PointLonLat H{h_ll, 0};

                for (size_t cnt = 0; cnt < Niter; ++cnt) {
                    Point2 M    = Point2::middle(A, B);
                    double dMdx = derivate->d(M).lon;

                    if (is_strictly_greater(dAdx * dMdx, 0.)) {
                        A    = M;
                        dAdx = dMdx;
                    }
                    else if (is_strictly_greater(dBdx * dMdx, 0.)) {
                        B    = M;
                        dBdx = dMdx;
                    }
                    else {
                        break;
                    }
                }

                // update extrema, extended by 'a small amount' (arbitrary)
                bounds.extend(std::get<PointLonLat>(projection.inv(Point2::middle(A, B))), H);
            }
        }
    }


    // 4. return bounding box
    return area::BoundingBox{bounds};
}


}  // namespace eckit::geo::util
