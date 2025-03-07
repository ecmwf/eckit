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

#include "eckit/geo/Area.h"


namespace eckit::geo {


class None final : public Area {
public:

    // -- Constructors

    None() = default;

    // -- Overriden methods

    const std::string& type() const override;

    bool intersects(area::BoundingBox&) const override { return false; }

private:

    // -- Overriden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo
