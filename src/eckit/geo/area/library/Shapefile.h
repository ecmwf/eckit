// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/area/Library.h"


namespace eckit::geo {
class Area;
}


namespace eckit::geo::area::library {


/**
 * @brief Shapefile areas (by entity index, or name), as area type shapefile
 * @details Spec: shapefile from 'url' (downloaded and cached) or 'file' (.shp, or .zip with one .shp), optionally
 * 'dbf', and the entity by 'index', or 'name' in field 'name_field'.
 */
class Shapefile : public Library {
public:

    // -- Constructors

    explicit Shapefile(const spec::Spec&);
    explicit Shapefile(const PathName&);
    explicit Shapefile(const PathName& shp, const PathName& dbf, const std::string& name = "");

    Shapefile(const Shapefile&) = delete;
    Shapefile(Shapefile&&)      = delete;

    // -- Destructor

    ~Shapefile() override;

    // -- Operators

    void operator=(const Shapefile&) = delete;
    void operator=(Shapefile&&)      = delete;

    // -- Overridden methods

    std::ostream& list(std::ostream&) const override;
    size_t size() const override { return static_cast<size_t>(nEntities_); }

    [[nodiscard]] Area* make_area(size_t) const override;
    [[nodiscard]] Area* make_area_from_name(const std::string&) const override;

    // -- Class methods

    [[nodiscard]] static Shapefile* make_from_url(const std::string&);

private:

    // -- Types

    struct Implementation;

    // -- Members

    std::string url_;
    const PathName file_;
    const PathName dbf_;

    const PathName shpPath_;
    const PathName dbfPath_;

    std::unique_ptr<Implementation> impl_;

    std::string name_;
    int nEntities_;

    std::map<std::string, int> to_entity;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Methods

    [[nodiscard]] Area* make_area(size_t, const std::string& name) const;
};


}  // namespace eckit::geo::area::library
