// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates using PROJ
class PROJ : public Projection {
public:

    // -- Constructors

    PROJ(const std::string& source, const std::string& target, double lon_minimum = 0.);
    explicit PROJ(const std::string& target, double lon_minimum = 0.) : PROJ(proj_default(), target, lon_minimum) {}
    explicit PROJ(const Spec&);

    // -- Destructor

    ~PROJ() override;

    // -- Methods

    const std::string& source() const { return source_; }
    const std::string& target() const { return target_; }

    // -- Overridden methods

    const std::string& type() const override;

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    // -- Class methods

    static std::string proj_str(const spec::Custom&);
    static const std::string& proj_default();

    /// If PROJ has a usable database.
    /// @return true if PROJ has a usable database, false otherwise
    static bool projdb_is_available();

    /// Set PROJ search paths for the database.
    /// @param db_path Path to the PROJ database.
    /// @param search_paths Additional search paths for the PROJ database.
    static void projdb_set_search_paths(const std::string& db_path, const std::vector<std::string>& search_paths = {});

    /// Reset PROJ search paths to the default values.
    static void projdb_reset();

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
