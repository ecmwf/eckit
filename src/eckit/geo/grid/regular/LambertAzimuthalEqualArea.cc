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


#include "eckit/geo/grid/regular/LambertAzimuthalEqualArea.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/etc/Grid.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/spec/Custom.h"
// #include "eckit/geo/util/regex.h"
// #include "eckit/types/Fraction.h"
// #include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


static const GridRegisterType<LambertAzimuthalEqualArea> __grid_type("lambert_azimuthal_equal_area");


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Spec& spec) :
    LambertAzimuthalEqualArea(Internal(spec)) {}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(size_t ni, size_t nj, double Dx, double Dy) :
    Regular(area::BoundingBox{}), Nx_(ni), Ny_(nj), Dx_(Dx), Dy_(Dy) {}


Grid::iterator LambertAzimuthalEqualArea::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator LambertAzimuthalEqualArea::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(Internal&& internal) :
    LambertAzimuthalEqualArea(internal.Nx, internal.Ny, internal.Dx, internal.Dy) {}


const std::vector<double>& LambertAzimuthalEqualArea::longitudes() const {
    NOTIMP;
}


const std::vector<double>& LambertAzimuthalEqualArea::latitudes() const {
    NOTIMP;
}


void LambertAzimuthalEqualArea::spec(spec::Custom& custom) const {
    // FIXME a lot more stuff to add here!
    spec::Custom spec({
        {"type", "lambert_azimuthal_equal_area"},
        {"grid", std::vector<double>{Dx_, Dy_}},
        {"dimensions", std::vector<long>{static_cast<long>(Nx_), static_cast<long>(Ny_)}},
        // ...
    });

    if (std::string name; SpecByName::instance().match(spec, name)) {
        custom.set("grid", name);
        return;
    }

    NOTIMP;
}


LambertAzimuthalEqualArea::Internal::Internal(const Spec& spec) {
    auto dimensions(spec.get_unsigned_vector("dimensions", {0, 0}));
    Nx = dimensions.size() == 2 ? dimensions[0] : spec.get_unsigned("Nx");
    Ny = dimensions.size() == 2 ? dimensions[1] : spec.get_unsigned("Ny");
    ASSERT(Nx > 0);
    ASSERT(Ny > 0);

    auto grid(spec.get_double_vector("grid", {0., 0.}));
    Dx = grid.size() == 2 ? grid[0] : spec.get_double("Nx");
    Dy = grid.size() == 2 ? grid[1] : spec.get_double("Ny");
    ASSERT(Dx > 0.);
    ASSERT(Dy > 0.);
}


}  // namespace eckit::geo::grid::regular
