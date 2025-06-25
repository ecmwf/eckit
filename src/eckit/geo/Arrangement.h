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

#include <string>


namespace eckit::geo {


enum Arrangement {
    FESOM_C,
    FESOM_N,

    ICON_C,
    ICON_T,
    ICON_N,
    ICON_E,

    ORCA_F,
    ORCA_T,
    ORCA_U,
    ORCA_V,
    ORCA_W,
};


}  // namespace eckit::geo
