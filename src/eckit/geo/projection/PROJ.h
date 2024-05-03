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

#include <memory>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates using PROJ
class PROJ final : public Projection {
public:
    // -- Exceptions
    // None

    // -- Constructors

    PROJ(const std::string& source, const std::string& target, double lon_minimum = 0.);
    explicit PROJ(const Spec&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    const std::string& source() const { return source_; }
    const std::string& target() const { return target_; }

    static std::string ellipsoid(const std::string& string);

    // -- Overridden methods

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    Spec* spec() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    struct Implementation;
    struct Convert;

    // -- Members

    std::unique_ptr<Implementation> implementation_;

    const std::string source_;
    const std::string target_;

    std::unique_ptr<Convert> from_;
    std::unique_ptr<Convert> to_;

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


}  // namespace eckit::geo::projection
