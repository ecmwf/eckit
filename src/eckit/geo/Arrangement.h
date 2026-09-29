// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once


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


}  // namespace eckit::geo
