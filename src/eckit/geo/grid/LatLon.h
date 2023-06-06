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

#include "eckit/geo/Grid.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


class LatLon : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    LatLon(const Configuration&);
    LatLon(const Increments&, const BoundingBox& = {}, const PointLonLat& reference = {0, 0});

    // -- Destructor

    ~LatLon() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    size_t Ni() const { return ni_; }

    size_t Nj() const { return nj_; }

    static void correctBoundingBox(BoundingBox&, size_t& ni, size_t& nj, const Increments&,
                                   const PointLonLat& reference = {0, 0});

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members

    const Increments increments_;
    const PointLonLat reference_;
    size_t ni_;
    size_t nj_;

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream&) const override;

    bool sameAs(const Grid&) const override;

    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    size_t numberOfPoints() const override;

    // -- Class members

    class LatLonIterator {
        size_t ni_;
        size_t nj_;
        Fraction north_;
        Fraction west_;
        Fraction we_;
        Fraction ns_;
        size_t i_;
        size_t j_;
        Latitude latValue_;
        Longitude lonValue_;
        Fraction lat_;
        Fraction lon_;

    protected:
        size_t count_;
        bool first_;

        ~LatLonIterator();
        void print(std::ostream&) const;
        bool next(double&, double&);

    public:
        LatLonIterator(size_t ni, size_t nj, Latitude north, Longitude west, const Increments& increments);
    };

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void reorder(long scanningMode) const override;

    const LatLon* croppedGrid(const BoundingBox&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
