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

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/PointLonLat.h"


namespace eckit::geo {
class Area;
class Spec;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo {


struct Shapefile {
    // -- Types

    using Poly = std::vector<PointLonLat>;

    // -- Constructors

    explicit Shapefile(const Spec&);
    explicit Shapefile(const PathName&);

    // -- Methods

    std::ostream& list(std::ostream&) const;

    [[nodiscard]] Area* make_area_from_key_string(const std::string&);
    [[nodiscard]] Area* make_area_from_key_int(int);

    // -- Class methods

    [[nodiscard]] static Shapefile* make_from_url(const std::string&);
    [[nodiscard]] static Shapefile* make_from_zip(const PathName&);

private:
    // -- Members

    PathName shp_;
    PathName dbf_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const;
};


}  // namespace eckit::geo
