// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Rotation of the sphere, given the location of its South Pole and an angle: fwd from (lon, lat) to (lon, lat) on the
/// rotated sphere (as PROJ ob_tran), inv the reverse (e.g. the grid points, from the rotated sphere)
class Rotation : public Projection {
public:

    // -- Constructors

    explicit Rotation(const Spec&);
    explicit Rotation(const PointLonLat& = SOUTH_POLE, double angle = 0);

    // -- Methods

    PointLonLat south_pole() const { return south_pole_; }
    double angle() const { return angle_; }
    bool rotated() const { return rotated_; }

    using Projection::fwd;
    using Projection::inv;

    inline PointLonLat fwd(const PointLonLat& p) const { return (*fwd_)(p); }
    inline PointLonLat inv(const PointLonLat& q) const { return (*inv_)(q); }

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointLonLat>(q)); }

    // -- Class methods

    [[nodiscard]] static Rotation* make_from_spec(const Spec&);

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    std::vector<std::vector<double>> fwd_vector(const std::vector<double>& lon, const std::vector<double>& lat,
                                                const std::vector<double>&) const override {
        return (*fwd_)(lon, lat);
    }

    std::vector<std::vector<double>> inv_vector(const std::vector<double>& lon, const std::vector<double>& lat,
                                                const std::vector<double>&) const override {
        return (*inv_)(lon, lat);
    }

private:

    // -- Types

    struct Implementation {
        Implementation()          = default;
        virtual ~Implementation() = default;

        Implementation(const Implementation&) = delete;
        Implementation(Implementation&&)      = delete;
        void operator=(const Implementation&) = delete;
        void operator=(Implementation&&)      = delete;

        virtual PointLonLat operator()(const PointLonLat&) const = 0;

        /// Rotate points, given/returning one vector per coordinate (lon, lat)
        virtual std::vector<std::vector<double>> operator()(const std::vector<double>& lon,
                                                            const std::vector<double>& lat) const {
            std::vector<std::vector<double>> out{std::vector<double>(lon.size()), std::vector<double>(lon.size())};
            for (size_t i = 0; i < lon.size(); ++i) {
                const auto q = operator()(PointLonLat{lon[i], lat[i]});
                out[0][i]    = q.lon();
                out[1][i]    = q.lat();
            }
            return out;
        }
    };

    // -- Members

    std::shared_ptr<Implementation> fwd_;
    std::shared_ptr<Implementation> inv_;

    PointLonLat south_pole_;
    double angle_;
    bool rotated_;
};


}  // namespace eckit::geo::projection
