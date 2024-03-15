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


#include "eckit/geo/grid/regular/Mercator.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid::regular {


static const GridRegisterType<Mercator> __grid_type("mercator");


Mercator::Mercator(const Spec& spec) :
    Mercator(Internal(spec)) {}


Mercator::Mercator(size_t Nx, size_t Ny, double Dx, double Dy) :
    Regular(area::BoundingBox{}), Nx_(Nx), Ny_(Ny), Dx_(Dx), Dy_(Dy) {}


Grid::iterator Mercator::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator Mercator::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}

const std::vector<double>& Mercator::longitudes() const {
    NOTIMP;
}


const std::vector<double>& Mercator::latitudes() const {
    NOTIMP;
}


void Mercator::spec(spec::Custom& custom) const {
    custom.set("type", "mercator");
    custom.set("grid", std::vector<double>{Dx_, Dy_});
    custom.set("shape", std::vector<long>{static_cast<long>(Nx_), static_cast<long>(Ny_)});

    // FIXME a lot more stuff to add here!
    //...

    NOTIMP;

    if (std::string name; SpecByName::instance().match(custom, name)) {
        custom.clear();
        custom.set("grid", name);
    }
}


Mercator::Internal::Internal(const Spec& spec) {
    auto shape(spec.get_unsigned_vector("shape", {0, 0}));
    Nx = shape.size() == 2 ? shape[0] : spec.get_unsigned("Nx");
    Ny = shape.size() == 2 ? shape[1] : spec.get_unsigned("Ny");
    ASSERT(Nx > 0);
    ASSERT(Ny > 0);

    auto grid(spec.get_double_vector("grid", {0., 0.}));
    Dx = grid.size() == 2 ? grid[0] : spec.get_double("Nx");
    Dy = grid.size() == 2 ? grid[1] : spec.get_double("Ny");
    ASSERT(Dx > 0.);
    ASSERT(Dy > 0.);
}


}  // namespace eckit::geo::grid::regular
