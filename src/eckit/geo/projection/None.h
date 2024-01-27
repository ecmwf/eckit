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


#pragma once

#include <memory>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class None : public Projection {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit None();
    explicit None(const std::string& source, const std::string& target);
    explicit None(const Spec&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    Spec* spec() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    struct Implementation {
        Implementation()          = default;
        virtual ~Implementation() = default;

        Implementation(const Implementation&) = delete;
        Implementation(Implementation&&)      = delete;
        void operator=(const Implementation&) = delete;
        void operator=(Implementation&&)      = delete;

        virtual Point operator()(const Point&) const = 0;
    };

    struct NoPointConversion final : Implementation {
        Point operator()(const Point& p) const override { return p; }
    };

    struct Point2ToPointLonLat final : Implementation {
        Point operator()(const Point& p) const override {
            const auto& q = std::get<Point2>(p);
            return PointLonLat{q.X, q.Y};
        }
    };

    struct PointLonLatToPoint2 final : Implementation {
        Point operator()(const Point& p) const override {
            const auto& q = std::get<PointLonLat>(p);
            return Point2{q.lon, q.lat};
        }
    };

    // -- Members

    std::unique_ptr<Implementation> fwd_;
    std::unique_ptr<Implementation> inv_;

    // -- Methods
    // None

    // -- Overridden methods

    Point fwd(const Point& p) const override { return (*fwd_)(p); }
    Point inv(const Point& q) const override { return (*inv_)(q); }

    // -- Class members
    // None

    // -- Class methods

    static Implementation* new_point_conversion(const std::string& source, const std::string& target);

    // -- Friends

    friend struct PlateCaree;
};


struct PlateCaree final : Projection {
    explicit PlateCaree() = default;
    explicit PlateCaree(const Spec&) {}

    Spec* spec() const override;
    inline Point fwd(const Point& p) const override { return fwd_(p); }
    inline Point inv(const Point& q) const override { return inv_(q); }

private:
    None::Point2ToPointLonLat fwd_;
    None::PointLonLatToPoint2 inv_;
};


}  // namespace eckit::geo::projection
