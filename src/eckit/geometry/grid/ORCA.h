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
#include <memory>
#include <vector>

#include "eckit/geometry/Grid.h"


namespace eckit {
class PathName;
class URI;
}  // namespace eckit


namespace eckit::geometry::grid {


class ORCA final : public Grid {
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

    class Iterator final : public geometry::Iterator {
    public:
        explicit Iterator(const Grid&, size_t index = 0);

    private:
        bool operator==(const geometry::Iterator&) const override;
        bool operator++() override;
        bool operator+=(diff_t) override;
        explicit operator bool() const override;
        Point operator*() const override;

        size_t index() const override { return index_; }

        size_t index_;
        const size_t index_size_;

        const std::vector<double>& longitudes_;
        const std::vector<double>& latitudes_;
        const std::string uid_;
    };

    // -- Exceptions
    // None

    // -- Constructors

    explicit ORCA(const Configuration&);
    explicit ORCA(const URI&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void read(const PathName&);
    size_t write(const PathName&, const std::string& compression = "none");

    std::string uid(const Configuration& config) const;

    // -- Overridden methods
    // None

    // -- Class members

    static Configuration* config(const std::string& name);

    // -- Class methods
    // None

private:
    // -- Members

    std::string name_;
    std::string uid_;
    Arrangement arrangement_;

    std::vector<double> longitudes_;
    std::vector<double> latitudes_;

    std::array<std::int32_t, 2> dimensions_{-1, -1};
    std::array<std::int32_t, 4> halo_{-1, -1, -1, -1};
    std::array<double, 2> pivot_{-1, -1};
    std::vector<std::byte> flags_;

    // -- Methods

    size_t ni() const { return static_cast<size_t>(dimensions_[0]); }
    size_t nj() const { return static_cast<size_t>(dimensions_[1]); }

    std::vector<double> longitudes(size_t ring) const;

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    const area::BoundingBox& boundingBox() const override;

    size_t size() const override { return ni() * nj(); }

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }  // FIXME: not sure this is semanticaly correct
    bool isPeriodicWestEast() const override { return true; }

    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::grid
