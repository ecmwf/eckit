// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>


namespace eckit::geo {


enum Arrangement {
    NONE = 0,

    ARAKAWA_C_T,
    ARAKAWA_C_U,
    ARAKAWA_C_V,

    FESOM_C,
    FESOM_N,

    ICON_C,
    ICON_V,
    ICON_E,

    ORCA_F,
    ORCA_T,
    ORCA_U,
    ORCA_V,
    ORCA_W,
};


/// The arrangement of a grid type by its letter (case-insensitive), e.g. ORCA_T of ("ORCA", "T")
Arrangement arrangement_from_string(const std::string& grid, const std::string& letter);


/// The letter of an arrangement, e.g. "T" of ORCA_T
const std::string& arrangement_to_string(Arrangement);


}  // namespace eckit::geo
