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


#include "eckit/geo/area/BoundingBox.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/Composer.h"
#include "eckit/geo/projection/EquidistantCylindrical.h"
#include "eckit/geo/projection/Reverse.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


static const AreaRegisterType<BoundingBox> AREATYPE("bounding_box");


namespace {


PointLonLat longitude_in_range(double reference, const PointLonLat& p) {
    // keep longitude difference (to reference) range below +-180 degree
    auto lon = p.lon();
    while (lon > reference + 180.) {
        lon -= 360.;
    }
    while (lon <= reference - 180.) {
        lon += 360.;
    }
    return {lon, p.lat()};
}


struct BoundLonLat {
    BoundLonLat(PointLonLat min, PointLonLat max) : min_(min), max_(max) {}

    explicit operator BoundingBox() const {
        // MIR-661 account for "excessive" bounds
        auto max_lon = std::min(max_.lon(), min_.lon() + PointLonLat::FULL_ANGLE);
        return {max_.lat(), min_.lon(), min_.lat(), max_lon};
    }

    void extend(PointLonLat p, PointLonLat eps) {
        ASSERT(0. <= eps.lon() && 0. <= eps.lat());

        auto sub = p - eps;
        auto add = p + eps;
        min_     = first_ ? sub : PointLonLat::componentsMin(min_, sub);
        max_     = first_ ? add : PointLonLat::componentsMax(max_, add);
        first_   = false;

        min_ = {min_.lon(), std::max(min_.lat(), -90.)};
        max_ = {std::min(max_.lon(), min_.lon() + 360.), std::min(max_.lat(), 90.)};
        ASSERT(min_.lon() <= max_.lon() && min_.lat() <= max_.lat());

        includesSouthPole(types::is_approximately_equal(min_.lat(), -90.));
        includesNorthPole(types::is_approximately_equal(max_.lat(), 90.));
        crossesDateLine(types::is_approximately_equal(max_.lon() - min_.lon(), 360.));
    }

    bool crossesDateLine(bool yes) {
        if ((crossesDateLine_ = crossesDateLine_ || yes)) {
            max_ = {min_.lon() + 360., max_.lat()};
        }
        return crossesDateLine_;
    }

    bool includesNorthPole(bool yes) {
        if ((includesNorthPole_ = includesNorthPole_ || yes)) {
            max_ = {max_.lon(), 90.};
        }
        crossesDateLine(includesNorthPole_);
        return includesNorthPole_;
    }

    bool includesSouthPole(bool yes) {
        if ((includesSouthPole_ = includesSouthPole_ || yes)) {
            min_ = {min_.lon(), -90.};
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
    Derivate(const Projection& p, PointXY A, PointXY B, double h, double refLongitude = 0.) :
        projection_(p),
        H_{PointXY::normalize(B - A) * h},
        invnH_(1. / PointXY::norm(H_)),
        refLongitude_(refLongitude) {}

    virtual ~Derivate() = default;

    Derivate(const Derivate&)       = delete;
    Derivate(Derivate&&)            = delete;
    void operator=(const Derivate&) = delete;
    void operator=(Derivate&&)      = delete;

    virtual PointLonLat d(PointXY) const = 0;

    PointLonLat f(const PointXY& p) const {
        return longitude_in_range(refLongitude_, std::get<PointLonLat>(projection_.inv(p)));
    }

    inline const PointXY& H() const { return H_; }
    inline double invnH() const { return invnH_; }

private:

    const Projection& projection_;
    const PointXY H_;
    const double invnH_;
    const double refLongitude_;
};


struct DerivateForwards final : Derivate {
    using Derivate::Derivate;
    PointLonLat d(PointXY P) const override { return (f(P + H()) - f(P)) * invnH(); }
};


struct DerivateBackwards final : Derivate {
    using Derivate::Derivate;
    PointLonLat d(PointXY P) const override { return (f(P) - f(P - H())) * invnH(); }
};


struct DerivateCentral final : Derivate {
    DerivateCentral(const Projection& p, PointXY A, PointXY B, double h, double refLongitude) :
        Derivate(p, A, B, h, refLongitude), H2_{H() * 0.5} {}
    const PointXY H2_;
    PointLonLat d(PointXY P) const override { return (f(P + H2_) - f(P - H2_)) * invnH(); }
};


struct DerivateFactory {
    static const Derivate* build(const std::string& type, const Projection& p, PointXY A, PointXY B, double h,
                                 double refLongitude = 0.) {
        ASSERT(0. < h);

        if (A.distance2(B) < h * h) {
            struct DerivateDegenerate final : Derivate {
                using Derivate::Derivate;
                PointLonLat d(PointXY) const override { return {99, 99}; }  // FIXME
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
    const Derivate* build_(const std::string& type, const Projection& p, PointXY A, PointXY B, double h,
                           double refLongitude) const;

    void list_(std::ostream& out) const { out << "forwards, backwards, central" << std::endl; }
};


[[nodiscard]] std::unique_ptr<BoundingBox> make_bounding_box(PointXY min, PointXY max, const Projection& projection,
                                                             double precision_ll, double precision_xy) {
    using types::is_strictly_greater;


    // 0. setup

    // use central longitude as absolute reference (keep points within +-180 longitude range)
    const PointXY centre_xy{(min.X() + max.X()) / 2., (min.Y() + max.Y()) / 2.};
    const auto centre_ll  = std::get<PointLonLat>(projection.inv(centre_xy));  // asserts fwd(PointLonLat) -> PointXY
    const auto centre_lon = centre_ll.lon();

    const std::string derivative_type = "central";
    const double h_ll                 = precision_ll;
    const double h                    = precision_xy;
    constexpr size_t Niter            = 100;


    // 1. determine box from projected corners

    struct : public std::pair<PointXY, PointXY> {
        using pair::pair;
        bool contains(const PointXY& P) const {
            return (first.X() < P.X() && P.X() < second.X()) && (first.Y() < P.Y() && P.Y() < second.Y());
        }
    } rect(min, max);

    const std::pair<PointXY, PointXY> segments[] = {{{min.X(), max.Y()}, {max.X(), max.Y()}},
                                                    {{max.X(), max.Y()}, {max.X(), min.Y()}},
                                                    {{max.X(), min.Y()}, {min.X(), min.Y()}},
                                                    {{min.X(), min.Y()}, {min.X(), max.Y()}}};

    BoundLonLat bounds(centre_ll, centre_ll);
    for (const auto& [A, dummy] : segments) {
        auto q = longitude_in_range(centre_lon, std::get<PointLonLat>(projection.inv(A)));
        bounds.extend(q, PointLonLat{h_ll, h_ll});
    }


    // 2. locate latitude extrema by checking if poles are included (in the un-projected frame) and if not, find extrema
    // not at the corners by refining iteratively

    if (!bounds.includesNorthPole()) {
        bounds.includesNorthPole(rect.contains(std::get<PointXY>(projection.fwd(PointLonLat{0., 90. - h_ll}))));
    }

    if (!bounds.includesSouthPole()) {
        bounds.includesSouthPole(rect.contains(std::get<PointXY>(projection.fwd(PointLonLat{0., -90. + h_ll}))));
    }

    for (auto [A, B] : segments) {
        if (!bounds.includesNorthPole() || !bounds.includesSouthPole()) {
            std::unique_ptr<const Derivate> derivate(
                DerivateFactory::build(derivative_type, projection, A, B, h, centre_lon));

            double dAdy = derivate->d(A).lat();
            double dBdy = derivate->d(B).lat();

            if (!is_strictly_greater(dAdy * dBdy, 0.)) {
                PointLonLat H{0, h_ll};

                for (size_t cnt = 0; cnt < Niter; ++cnt) {
                    PointXY M   = PointXY::middle(A, B);
                    double dMdy = derivate->d(M).lat();
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
                bounds.extend(std::get<PointLonLat>(projection.inv(PointXY::middle(A, B))), H);
            }
        }
    }


    // 3. locate longitude extrema not at the corners by refining iteratively

    for (auto [A, B] : segments) {
        if (!bounds.crossesDateLine()) {
            std::unique_ptr<const Derivate> derivate(
                DerivateFactory::build(derivative_type, projection, A, B, h, centre_lon));

            double dAdx = derivate->d(A).lon();
            double dBdx = derivate->d(B).lon();

            if (!is_strictly_greater(dAdx * dBdx, 0.)) {
                PointLonLat H{h_ll, 0};

                for (size_t cnt = 0; cnt < Niter; ++cnt) {
                    PointXY M   = PointXY::middle(A, B);
                    double dMdx = derivate->d(M).lon();

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
                bounds.extend(std::get<PointLonLat>(projection.inv(PointXY::middle(A, B))), H);
            }
        }
    }


    // 4. return bounding box
    return std::make_unique<BoundingBox>(bounds);
}


}  // namespace


static inline bool is_approximately_equal(BoundingBox::value_type a, BoundingBox::value_type b) {
    return types::is_approximately_equal(a, b, PointLonLat::EPS);
}


std::unique_ptr<BoundingBox> BoundingBox::make_global_prime() {
    return std::make_unique<BoundingBox>(PointLonLat::RIGHT_ANGLE, 0., -PointLonLat::RIGHT_ANGLE,
                                         PointLonLat::FULL_ANGLE);
}


std::unique_ptr<BoundingBox> BoundingBox::make_global_antiprime() {
    return std::make_unique<BoundingBox>(PointLonLat::RIGHT_ANGLE, -PointLonLat::FLAT_ANGLE, -PointLonLat::RIGHT_ANGLE,
                                         PointLonLat::FLAT_ANGLE);
}


void BoundingBox::fill_spec(spec::Custom& custom) const {
    custom.set("area", std::vector<double>{north(), west(), south(), east()});
}


std::unique_ptr<BoundingBox> BoundingBox::make_from_spec(const Spec& spec) {
    auto [n, w, s, e] = bounding_box_default().deconstruct();

    if (std::vector<double> area{n, w, s, e}; spec.get("area", area) || spec.get("bounding_box", area)) {
        ASSERT_MSG(area.size() == 4, "BoundingBox: 'area'/'bounding_box' expected list of size 4");
        return make_from_area(area[0], area[1], area[2], area[3]);
    }

    spec.get("north", n);
    spec.get("south", s);

    if (spec.get("west", w) && !spec.has("east")) {
        e = w + PointLonLat::FULL_ANGLE;
    }

    if (spec.get("east", e) && !spec.has("west")) {
        w = e - PointLonLat::FULL_ANGLE;
    }

    return make_from_area(n, w, s, e);
}


std::unique_ptr<BoundingBox> BoundingBox::make_from_projection(PointXY min, PointXY max, const Projection& projection) {
    constexpr double precision_ll = 0.5e-6;  // to microdegrees
    constexpr double precision_xy = 0.5e-1;  // to decimeters

    return make_bounding_box(min, max, projection, precision_ll, precision_xy);
}


std::unique_ptr<BoundingBox> BoundingBox::make_from_projection(PointLonLat min, PointLonLat max,
                                                               const projection::Rotation& rotation) {
    projection::Composer projection{new projection::Reverse<projection::Rotation>(rotation.spec()),
                                    new projection::EquidistantCylindrical};

    auto after = make_from_projection(PointXY{min.lon(), min.lat()}, PointXY{max.lon(), max.lat()}, projection);
    if (after->periodic()) {
        return std::make_unique<BoundingBox>(after->north(), 0, after->south(), PointLonLat::FULL_ANGLE);
    }

    return after;
}


std::unique_ptr<BoundingBox> BoundingBox::make_from_area(value_type n, value_type w, value_type s, value_type e) {
    // set latitudes inside usual range (not a normalisation like PointLonLat::make)
    if (n > NORTH_POLE.lat() || is_approximately_equal(n, NORTH_POLE.lat())) {
        n = NORTH_POLE.lat();
    }

    if (s < SOUTH_POLE.lat() || is_approximately_equal(s, SOUTH_POLE.lat())) {
        s = SOUTH_POLE.lat();
    }

    // normalise west in [min, min + 2 pi[, east in [west, west + 2 pi[
    const auto min  = -PointLonLat::FLAT_ANGLE;
    const auto same = is_approximately_equal(w, e);

    w = is_approximately_equal(w, min) || is_approximately_equal(w, min + PointLonLat::FULL_ANGLE)
            ? min
            : PointLonLat::normalise_angle_to_minimum(w, min);

    auto a = PointLonLat::normalise_angle_to_minimum(e, w);
    e      = same ? w : is_approximately_equal(w, a) ? (w + PointLonLat::FULL_ANGLE) : a;

    return std::make_unique<BoundingBox>(n, w, s, e);
}


BoundingBox::BoundingBox(const Spec& spec) : BoundingBox(*make_from_spec(spec)) {}


BoundingBox::BoundingBox(value_type n, value_type w, value_type s, value_type e) : array{n, w, s, e} {
    auto is_approx_ge = [](value_type a, value_type b) { return is_approximately_equal(a, b) || a > b; };

    // normalise east in [west, west + 2 pi[
    auto a        = PointLonLat::normalise_angle_to_minimum(e, w);
    operator[](3) = is_approximately_equal(w, e)                   ? w
                    : is_approximately_equal(w, a)                 ? w + PointLonLat::FULL_ANGLE
                    : is_approx_ge(e - w, PointLonLat::FULL_ANGLE) ? w + PointLonLat::FULL_ANGLE
                                                                   : a;

    ASSERT(south() <= north());
    ASSERT(west() <= east());
}


BoundingBox::BoundingBox() : BoundingBox(*make_global_prime()) {}


bool BoundingBox::global() const {
    return periodic() && contains(NORTH_POLE) && contains(SOUTH_POLE);
}


bool BoundingBox::periodic() const {
    return west() != east() && is_approximately_equal(west(), PointLonLat::normalise_angle_to_minimum(east(), west()));
}


bool BoundingBox::contains(const Point& p) const {
    const auto& q = std::get<PointLonLat>(p);

    // NOTE: latitudes < -90 or > 90 are not considered
    if (is_approximately_equal(q.lat(), NORTH_POLE.lat())) {
        return is_approximately_equal(q.lat(), north());
    }

    if (is_approximately_equal(q.lat(), SOUTH_POLE.lat())) {
        return is_approximately_equal(q.lat(), south());
    }

    if ((south() < q.lat() && q.lat() < north()) || is_approximately_equal(q.lat(), north()) ||
        is_approximately_equal(q.lat(), south())) {
        return PointLonLat::normalise_angle_to_minimum(q.lon(), west()) <= east();
    }

    return false;
}


const BoundingBox& BoundingBox::bounding_box_default() {
    static const BoundingBox bbox;
    return bbox;
}


bool BoundingBox::contains(const BoundingBox& other) const {
    if (other.empty()) {
        return contains(PointLonLat{other.south(), other.west()});
    }

    // check for West/East range (if non-periodic), then other's corners
    if (east() - west() < other.east() - other.west() ||
        east() < PointLonLat::normalise_angle_to_minimum(other.east(), west())) {
        return false;
    }

    return contains(PointLonLat{other.north(), other.west()}) && contains(PointLonLat{other.north(), other.east()}) &&
           contains(PointLonLat{other.south(), other.west()}) && contains(PointLonLat{other.south(), other.east()});
}


bool BoundingBox::intersects(BoundingBox& other) const {
    auto n = std::min(north(), other.north());
    auto s = std::max(south(), other.south());

    bool intersectsSN = s <= n;
    if (!intersectsSN) {
        n = s;
    }

    if (periodic() && other.periodic()) {
        other = {n, other.west(), s, other.east()};
        return intersectsSN;
    }

    auto w = std::min(west(), other.west());
    auto e = w;

    auto intersect = [](const BoundingBox& a, const BoundingBox& b, value_type& w, value_type& e) {
        bool p = a.periodic();
        if (p || b.periodic()) {
            w = (p ? b : a).west();
            e = (p ? b : a).east();
            return true;
        }

        auto ref = PointLonLat::normalise_angle_to_minimum(b.west(), a.west());
        auto w_  = std::max(a.west(), ref);
        auto e_  = std::min(a.east(), PointLonLat::normalise_angle_to_minimum(b.east(), ref));

        if (w_ <= e_) {
            w = w_;
            e = e_;
            return true;
        }

        return false;
    };

    bool intersectsWE = west() <= other.west() ? intersect(*this, other, w, e) || intersect(other, *this, w, e)
                                               : intersect(other, *this, w, e) || intersect(*this, other, w, e);

    ASSERT_MSG(w <= e, "BoundingBox::intersects: longitude range");
    other = {n, w, s, e};

    return intersectsSN && intersectsWE;
}


bool BoundingBox::empty() const {
    return is_approximately_equal(north(), south()) || is_approximately_equal(west(), east());
}


double BoundingBox::area() const {
    return figure::Earth::_area(*this);
}


const std::string& BoundingBox::type() const {
    static const std::string type{"bounding-box"};
    return type;
}


bool bounding_box_equal(const BoundingBox& a, const BoundingBox& b) {
    const auto c = BoundingBox::make_from_area(a.north(), a.west(), a.south(), a.east());
    ASSERT(c);

    const auto d = BoundingBox::make_from_area(b.north(), b.west(), b.south(), b.east());
    ASSERT(c);

    return is_approximately_equal(c->north(), d->north()) && is_approximately_equal(c->south(), d->south()) &&
           is_approximately_equal(c->west(), d->west()) && is_approximately_equal(c->east(), d->east());
}


}  // namespace eckit::geo::area
