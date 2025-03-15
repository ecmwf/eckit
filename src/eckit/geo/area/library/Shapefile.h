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
#include <map>
#include <string>

#include "eckit/filesystem/PathName.h"

#include "shapefil.h"


namespace eckit::geo {
class Area;
class Spec;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo::area::library {


struct Shapefile {
    // -- Constructors

    explicit Shapefile(const Spec&);
    explicit Shapefile(const PathName&);
    explicit Shapefile(const PathName& shp, const PathName& dbf, const std::string& name_field = "");

    // -- Destructor

    ~Shapefile();

    // -- Methods

    std::ostream& list(std::ostream&) const;
    size_t size() const { return static_cast<size_t>(nEntities_); }

    [[nodiscard]] Area* make_area(size_t) const;
    [[nodiscard]] Area* make_area_from_name(const std::string&) const;

    // -- Class methods

    [[nodiscard]] static Shapefile* make_from_url(const std::string&);

private:
    // -- Members

    const PathName shpPath_;
    const PathName dbfPath_;

    SHPInfo* shp_;

    std::string name_;
    int nEntities_;

    std::map<std::string, int> to_entity;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const;
};


}  // namespace eckit::geo::area::library
