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


#define eckit_HAVE_GEO_AREA_SHAPEFILE 0


constexpr bool _to_bool(int x) {
    return x != 0;
}

constexpr bool eckit_HAVE_ECKIT_CODEC     = _to_bool(1);
#define eckit_HAVE_PROJ 0

constexpr bool eckit_HAVE_GEO_CACHING                 = _to_bool(0);
constexpr bool eckit_HAVE_GEO_BITREPRODUCIBLE         = _to_bool(0);
constexpr bool eckit_HAVE_GEO_PROJECTION_PROJ_DEFAULT = _to_bool(0);

constexpr const char* eckit_GEO_CACHE_PATH       = "~/.local/share/eckit/geo";
constexpr const char* eckit_GEO_SHARE_URL_PREFIX = "https://sites.ecmwf.int/repository/eckit/geo";

constexpr const char* eckit_GEO_SHARE_AREA       = "~eckit/share/eckit/geo/area.yaml";
constexpr const char* eckit_GEO_SHARE_GRID       = "~eckit/share/eckit/geo/grid.yaml:~eckit/share/eckit/geo/FESOM.yaml:~eckit/share/eckit/geo/ICON-CH.yaml:~eckit/share/eckit/geo/ICON-DWD.yaml:~eckit/share/eckit/geo/ICON-MPIM.yaml:~eckit/share/eckit/geo/ORCA.yaml";
constexpr const char* eckit_GEO_SHARE_PROJECTION = "~eckit/share/eckit/geo/projection.yaml";
