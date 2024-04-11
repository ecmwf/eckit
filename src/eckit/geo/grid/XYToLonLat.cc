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


#include "eckit/geo/grid/XYToLonLat.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/etc/Grid.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid {


namespace {


std::array<double, 2> grid_from_spec(const Spec& spec) {
    std::array<double, 2> grid{0, 0};
    if (std::vector<double> value; spec.get("grid", value) && value.size() == 2) {
        grid[0] = value[0];
        grid[1] = value[1];
    }
    else if (!spec.get("dx", grid[0]) || !spec.get("dy", grid[1])) {
        throw SpecNotFound("'grid' = ['dx', 'dy'] expected");
    }

    if (!(grid[0] > 0) || !(grid[1] > 0)) {
        throw BadValue("'grid' = ['dx', 'dy'] > 0 expected");
    }

    return grid;
}


std::array<size_t, 2> shape_from_spec(const Spec& spec) {
    std::array<size_t, 2> shape{0, 0};
    if (std::vector<size_t> value; spec.get("shape", value) && value.size() == 2) {
        shape[0] = value[0];
        shape[1] = value[1];
    }
    else if (!spec.get("nx", shape[0]) || !spec.get("ny", shape[1])) {
        throw SpecNotFound("'shape' = ['nx', 'ny'] expected");
    }

    if (!(shape[0] > 0) || !(shape[1] > 0)) {
        throw BadValue("'shape' = ['nx', 'ny'] > 0 expected");
    }

    return shape;
}


}  // namespace


XYToLonLat::XYToLonLat(const Spec& spec) :
    Regular(spec), grid_(grid_from_spec(spec)), shape_(shape_from_spec(spec)) {}


Grid::iterator XYToLonLat::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator XYToLonLat::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


const std::vector<double>& XYToLonLat::longitudes() const {
    NOTIMP;
}


const std::vector<double>& XYToLonLat::latitudes() const {
    NOTIMP;
}


static const GridRegisterType<LambertAzimuthalEqualArea> __grid_type1("lambert_azimuthal_equal_area");
static const GridRegisterType<Mercator> __grid_type2("mercator");


void Mercator::spec(spec::Custom& custom) const {
    custom.set("type", "mercator");
    custom.set("grid", std::vector<double>{dy(), dx()});
    custom.set("shape", std::vector<long>{static_cast<long>(ny()), static_cast<long>(nx())});

    // FIXME a lot more stuff to add here!
    //...

    if (std::string name; SpecByName::instance().match(custom, name)) {
        custom.clear();
        custom.set("grid", name);
    }
}


void LambertAzimuthalEqualArea::spec(spec::Custom& custom) const {
    custom.set("type", "lambert_azimuthal_equal_area");
    custom.set("grid", std::vector<double>{dy(), dx()});
    custom.set("shape", std::vector<long>{static_cast<long>(ny()), static_cast<long>(nx())});

    // FIXME a lot more stuff to add here!
    //...

    if (std::string name; SpecByName::instance().match(custom, name)) {
        custom.clear();
        custom.set("grid", name);
    }
}


}  // namespace eckit::geo::grid
