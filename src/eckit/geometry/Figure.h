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


namespace eckit {
class Configuration;
}


namespace eckit::geometry {


class Figure {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Figure(const Configuration&);

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

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
};


struct FigureFactory {
    static Figure* build(const Configuration&);
    static Figure* build(const std::string&, const Configuration&);
    static std::ostream& list(std::ostream&);

    FigureFactory(const FigureFactory&)            = delete;
    FigureFactory(FigureFactory&&)                 = delete;
    FigureFactory& operator=(const FigureFactory&) = delete;
    FigureFactory& operator=(FigureFactory&&)      = delete;

    virtual Figure* make(const Configuration&) = 0;

protected:
    explicit FigureFactory(const std::string&);
    virtual ~FigureFactory();

private:
    const std::string key_;
};


template <class T>
class FigureBuilder final : public FigureFactory {
    Figure* make(const Configuration& config) override { return new T(config); }

public:
    explicit FigureBuilder(const std::string& key) :
        FigureFactory(key) {}
};


}  // namespace eckit::geometry
