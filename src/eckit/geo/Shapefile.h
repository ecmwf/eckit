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


namespace eckit::geo {


struct Shapefile {
    // -- Constructors

    explicit Shapefile(const Spec&);
    explicit Shapefile(const PathName& shp, const PathName& dbf = "");

    // -- Destructor

    ~Shapefile();

    // -- Methods

    std::ostream& list(std::ostream&) const;

    void set_name_field(const std::string&);
    [[nodiscard]] Area* make_area_from_name(const std::string&) const;

    size_t n_entities() const { return static_cast<size_t>(nEntities_); }
    [[nodiscard]] Area* make_area_from_entity(size_t) const;

    // -- Class methods

    [[nodiscard]] static Shapefile* make_from_url(const std::string&);

private:
    // -- Members

    const PathName shpPath_;
    const PathName dbfPath_;

    SHPInfo* shp_;

    std::string field_;
    int fieldIndex_;
    int nEntities_;

    std::map<std::string, int> to_entity;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const;
};


}  // namespace eckit::geo
