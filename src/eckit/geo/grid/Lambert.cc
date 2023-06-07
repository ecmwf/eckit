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


#include "eckit/geo/grid/Lambert.h"

#include "eckit/config/MappedConfiguration.h"


namespace eckit::geo::grid {


static const GridBuilder<Lambert> __builder("lambert");


Lambert::Lambert(const Configuration& config) :
    RegularGrid(config, make_projection(config)) {
    auto edition = config.getLong("edition", 0);

    // GRIB1 cannot write LaD
    writeLaDInDegrees_ = edition == 2;
    config.get("writeLaDInDegrees", writeLaDInDegrees_);

    // GRIB2 cannot write negative longitude values
    writeLonPositive_ = edition == 2;
    config.get("writeLonPositive", writeLonPositive_);
}


Projection* Lambert::make_projection(const Configuration& config) {
#if 0
    auto spec = make_proj_spec(config);
    if (!spec.empty()) {
        return spec;
    }
#endif

    auto LaDInDegrees    = config.getDouble("LaDInDegrees");
    auto LoVInDegrees    = config.getDouble("LoVInDegrees");
    auto Latin1InDegrees = config.getDouble("Latin1InDegrees", LaDInDegrees);
    auto Latin2InDegrees = config.getDouble("Latin2InDegrees", LaDInDegrees);

    return ProjectionFactory::build("lambert_conformal_conic",
                                    MappedConfiguration({{"latitude1", Latin1InDegrees},
                                                         {"latitude2", Latin2InDegrees},
                                                         {"latitude0", LaDInDegrees},
                                                         {"longitude0", LoVInDegrees}}));
}


}  // namespace eckit::geo::grid
