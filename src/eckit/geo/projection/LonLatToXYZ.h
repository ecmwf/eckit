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

#include "eckit/geo/projection/ProjectionOnFigure.h"


namespace eckit::geo::projection {


/// Calculate coordinates of a point on a sphere or spheroid, in [x, y, z]
class LonLatToXYZ : public ProjectionOnFigure {
public:

    // -- Constructors

    explicit LonLatToXYZ(Figure* = nullptr);

    explicit LonLatToXYZ(double R);
    explicit LonLatToXYZ(double a, double b);

    explicit LonLatToXYZ(const Spec&);

    // -- Methods

    inline PointXYZ fwd(const PointLonLat& p) const { return (*impl_)(p); }
    inline PointLonLat inv(const PointXYZ& q) const { return (*impl_)(q); }

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return (*impl_)(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return (*impl_)(std::get<PointXYZ>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Types

    struct Implementation {
        Implementation()          = default;
        virtual ~Implementation() = default;

        Implementation(const Implementation&) = delete;
        Implementation(Implementation&&)      = delete;
        void operator=(const Implementation&) = delete;
        void operator=(Implementation&&)      = delete;

        virtual PointXYZ operator()(const PointLonLat&) const = 0;
        virtual PointLonLat operator()(const PointXYZ&) const = 0;
    };

    // -- Members

    std::unique_ptr<Implementation> impl_;
};


}  // namespace eckit::geo::projection
