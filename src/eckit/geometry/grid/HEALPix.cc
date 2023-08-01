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


#include "eckit/geometry/grid/HEALPix.h"

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/area/BoundingBox.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid {


static const area::BoundingBox __global;


HEALPix::HEALPix(const Configuration& config) :
    HEALPix(config.getUnsigned("Nside")) {
}


HEALPix::HEALPix(size_t Nside) :
    Grid(__global), N_(Nside) {
    ASSERT(N_ > 0);
}


Configuration* HEALPix::config(const std::string& name) {
    auto Nside = Translator<std::string, size_t>{}(name.substr(1));
    return new MappedConfiguration({{"type", "healpix"}, {"Nside", Nside}});
}


const area::BoundingBox& HEALPix::boundingBox() const {
    return __global;
}


size_t HEALPix::size() const {
    return 12 * N_ * N_;
}


static const GridRegisterType<HEALPix> __grid_type("healpix");
static const GridRegisterName<HEALPix> __grid_pattern("[hH][1-9][0-9]*");


}  // namespace eckit::geometry::grid
