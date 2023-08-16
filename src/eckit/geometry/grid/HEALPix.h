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

#include <vector>

#include "eckit/geometry/Grid.h"


namespace eckit::geometry::grid {


class HEALPix final : public Grid {
public:
    // -- Types

    class RingIterator final : public geometry::Iterator {
    public:
        explicit RingIterator(const Grid&, size_t index = 0);

    private:
        bool operator==(const Iterator&) const override;
        bool operator++() override;
        bool operator+=(diff_t) override;
        explicit operator bool() const override;
        Point operator*() const override;

        size_t index() const override { return index_; }
        size_t j(size_t index) const;

        const HEALPix& grid_;
        std::vector<double> longitudes_j_;
        size_t j_;
        size_t index_;
        const size_t index_size_;
    };

    // -- Exceptions
    // None

    // -- Constructors

    explicit HEALPix(const Configuration&);
    explicit HEALPix(size_t Nside, Ordering = Ordering::healpix_ring);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members

    static Configuration* config(const std::string& name);

    // -- Class methods
    // None

private:
    // -- Members

    const size_t N_;
    const Ordering ordering_;

    std::vector<size_t> njacc_;

    mutable std::vector<double> latitudes_;

    // -- Methods

    size_t ni() const;
    size_t nj(size_t i) const;

    const std::vector<double>& latitudes() const;
    std::vector<double> longitudes(size_t ring) const;

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    const area::BoundingBox& boundingBox() const override;

    size_t size() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend class RingIterator;
};


}  // namespace eckit::geometry::grid
