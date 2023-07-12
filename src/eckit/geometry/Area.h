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
namespace geometry::area {
class BoundingBox;
}
}  // namespace eckit


namespace eckit::geometry {


class Area {
public:
    // -- Types

    using Type = std::string;

    // -- Exceptions
    // None

    // -- Constructors

    Area() noexcept = default;

    Area(const Area&) = default;
    Area(Area&&)      = default;

    // -- Destructor

    virtual ~Area() = default;

    // -- Convertors
    // None

    // -- Operators

    Area& operator=(const Area&) = default;
    Area& operator=(Area&&)      = default;

    // -- Methods

    virtual area::BoundingBox bbox() const = 0;

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


struct AreaFactory {
    static Area* build(const Configuration&);
    static Area* build(const Area::Type&, const Configuration&);
    static std::ostream& list(std::ostream&);

    AreaFactory(const AreaFactory&)            = delete;
    AreaFactory(AreaFactory&&)                 = delete;
    AreaFactory& operator=(const AreaFactory&) = delete;
    AreaFactory& operator=(AreaFactory&&)      = delete;

    virtual Area* make(const Configuration&) = 0;

protected:
    explicit AreaFactory(const Area::Type&);
    virtual ~AreaFactory();

private:
    const Area::Type key_;
};


template <class T>
class AreaBuilder final : public AreaFactory {
    Area* make(const Configuration& config) override { return new T(config); }

public:
    explicit AreaBuilder(const Area::Type& key) :
        AreaFactory(key) {}
};


}  // namespace eckit::geometry
