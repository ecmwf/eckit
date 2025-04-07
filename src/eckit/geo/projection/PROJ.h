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
class PROJ : public Projection {
public:

    // -- Constructors

    PROJ(const std::string& source, const std::string& target, double lon_minimum = 0.);
    explicit PROJ(const Spec&);

    // -- Methods

    const std::string& source() const { return source_; }
    const std::string& target() const { return target_; }

    // -- Overridden methods

    const std::string& type() const override;

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    [[nodiscard]] Figure* make_figure() const override;

    // -- Class methods

    static std::string proj_str(const spec::Custom&);

private:

    // -- Types

    struct Implementation;

    // -- Members

    std::unique_ptr<Implementation> implementation_;

    const std::string source_;
    const std::string target_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::projection
