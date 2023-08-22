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

#include "eckit/geometry/Grid.h"
#include "eckit/geometry/Point.h"


namespace eckit {
class Configuration;
}


namespace eckit::geometry::grid {


class Regular : public Grid {
public:
    // -- Types

#if 0
    struct Increments {
        template <typename U                                                     = P,
                  typename std::enable_if_t<std::is_same_v<U, PointLonLat>, int> = 0>
        explicit Increments(const Configuration& config) :
            P(config.getDouble("west_east_increment"), config.getDouble("south_north_increment")) {
        }

        template <typename U                                                = P,
                  typename std::enable_if_t<std::is_same_v<U, Point2>, int> = 0>
        explicit Increments(const Configuration& config) :
            P(config.getDouble("dx"), config.getDouble("dy")) {
        }

        std::array<double, 2> deconstruct() const { return {P::operator[](0), P::operator[](0)}; }

        const P point_;
    };
#else
    struct Increments : protected std::array<double, 2> {
        using P = std::array<double, 2>;

        Increments(double west_east, double south_north) :
            P{west_east, south_north} {};

        explicit Increments(const Configuration& config);

        double& west_east   = P::operator[](0);
        double& south_north = P::operator[](1);

        std::array<double, 2> deconstruct() const { return {west_east, south_north}; }
    };
#endif

    // -- Exceptions
    // None

    // -- Constructors

    explicit Regular(const Configuration&);
    explicit Regular(const Increments&, const area::BoundingBox& = {}, const PointLonLat& reference = {0, 0});

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static area::BoundingBox correctBoundingBox(const area::BoundingBox&, size_t& ni, size_t& nj, const Increments&,
                                                const PointLonLat& reference = {0, 0});

    static Configuration* config(const std::string& name);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const Increments increments_;
    const PointLonLat reference_;
    size_t ni_;
    size_t nj_;

    // -- Methods
    // None

    // -- Overridden methods

    iterator cbegin() const override { NOTIMP; }
    iterator cend() const override { NOTIMP; }

    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    size_t size() const override;

    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::grid
