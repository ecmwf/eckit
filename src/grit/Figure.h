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

#include <iosfwd>
#include <string>


#include "grit/types.h"


namespace grit {


class Figure {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit Figure(double R);
    explicit Figure(double a, double b);

    Figure(const Figure&) = default;
    Figure(Figure&&)      = default;

    // -- Destructor

    virtual ~Figure() = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    Figure& operator=(const Figure&) = default;
    Figure& operator=(Figure&&)      = default;

    // -- Methods

    bool sphere() const { return R_ == R_; }
    double a() const { return a_; }
    double b() const { return b_; }
    double R() const;

    /// Great-circle central angle between two points (latitude/longitude coordinates) in radians
    virtual double angle(const PointLatLon&, const PointLatLon&) const = 0;

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    virtual double angle(const PointXYZ&, const PointXYZ&) const = 0;

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    virtual double distance(const PointLatLon&, const PointLatLon&) const = 0;

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    virtual double distance(const PointXYZ&, const PointXYZ&) const = 0;

    /// Surface area in square metres
    virtual double area() const = 0;

    // Convert spherical coordinates to Cartesian
    virtual PointXYZ ll_to_xyz(const PointLatLon&, double height) const = 0;

    // Convert Cartesian coordinates to spherical
    virtual PointLatLon xyz_to_ll(const PointXYZ&) const = 0;

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

private:
    // -- Members

    double a_;
    double b_;
    double R_;

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


struct FigureFactory {
    using key_type = std::string;

    Figure* build(const key_type&);
    static void list(std::ostream&);

    FigureFactory(const FigureFactory&)            = delete;
    FigureFactory(FigureFactory&&)                 = delete;
    FigureFactory& operator=(const FigureFactory&) = delete;
    FigureFactory& operator=(FigureFactory&&)      = delete;

    virtual Figure* make() = 0;

protected:
    explicit FigureFactory(const key_type&);
    virtual ~FigureFactory();

private:
    const key_type key_;
};


template <class T>
class FigureBuilder final : public FigureFactory {
    Figure* make() override { return new T; }

public:
    explicit FigureBuilder(const FigureFactory::key_type& key) : FigureFactory(key) {}
};


}  // namespace grit
