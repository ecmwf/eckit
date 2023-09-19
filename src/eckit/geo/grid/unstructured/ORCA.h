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

#include <array>
#include <cstddef>
#include <cstdint>

#include "eckit/geo/grid/Unstructured.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::grid::unstructured {


class ORCA final : public Unstructured {
public:
    // -- Types

    enum Arrangement
    {
        F,
        T,
        U,
        V,
        W,
    };

    // -- Exceptions
    // None

    // -- Constructors

    explicit ORCA(const Configuration&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    uid_t calculate_uid() const;

    void read(const PathName&);
    void check(const Configuration&);
    size_t write(const PathName&, const std::string& compression = "none");

    size_t ni() const { return static_cast<size_t>(dimensions_[0]); }
    size_t nj() const { return static_cast<size_t>(dimensions_[1]); }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    const area::BoundingBox& boundingBox() const override;

    size_t size() const override { return ni() * nj(); }
    uid_t uid() const override { return uid_; }

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }  // FIXME: not sure this is semanticaly correct
    bool isPeriodicWestEast() const override { return true; }

    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    const std::vector<double>& longitudes() const override { return longitudes_; }
    const std::vector<double>& latitudes() const override { return latitudes_; }

    // -- Class members
    // None

    // -- Class methods

    static Configuration* config(const std::string& name);

private:
    // -- Members

    std::string name_;
    uid_t uid_;
    Arrangement arrangement_;

    std::array<std::int32_t, 2> dimensions_;
    std::array<std::int32_t, 4> halo_;
    std::array<double, 2> pivot_;
    std::vector<double> longitudes_;
    std::vector<double> latitudes_;
    std::vector<std::byte> flags_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::unstructured
