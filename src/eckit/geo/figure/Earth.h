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

#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"


namespace eckit::geo::figure {


struct DatumIFS {
    static constexpr double radius = 6371229.;
};


struct DatumGRIB1 {
    static constexpr double radius = 6367470.;
};


struct DatumGRS80 {
    static constexpr double a = 6378137.;
    static constexpr double b = 6356752.314140;
};


struct DatumWGS84 {
    static constexpr double a = 6378137.;
    static constexpr double b = 6356752.314245;
};


struct Earth final : public Sphere {
    explicit Earth() : Sphere(DatumIFS::radius) {}
    explicit Earth(const Spec&) : Earth() {}
};


struct GRS80 final : public OblateSpheroid {
    explicit GRS80() : OblateSpheroid(DatumGRS80::a, DatumGRS80::b) {}
    explicit GRS80(const Spec&) : GRS80() {}
};


struct WGS84 final : public OblateSpheroid {
    explicit WGS84() : OblateSpheroid(DatumWGS84::a, DatumWGS84::b) {}
    explicit WGS84(const Spec&) : WGS84() {}
};


}  // namespace eckit::geo::figure
