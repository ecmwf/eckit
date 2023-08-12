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


namespace eckit::geometry {


class Domain {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Domain(double north, double west, double south, double east);
    Domain();
    Domain(const Domain&) = default;
    Domain(Domain&&)      = default;

    // -- Destructor

    virtual ~Domain() = default;

    // -- Convertors
    // None

    // -- Operators

    Domain& operator=(const Domain&) = default;
    Domain& operator=(Domain&&)      = default;
    bool operator==(const Domain&) const;
    bool operator!=(const Domain& other) const { return !operator==(other); }

    // -- Methods

    double north() const { return north_; }
    double west() const { return west_; }
    double south() const { return south_; }
    double east() const { return east_; }

    bool includesNorthPole() const;
    bool includesSouthPole() const;
    bool isPeriodicWestEast() const;

    bool contains(double lat, double lon) const;
    bool contains(const Domain&) const;
    bool empty() const;
    double area(double radius) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

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

private:
    // -- Members

    double north_;
    double west_;
    double south_;
    double east_;

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


}  // namespace eckit::geometry
