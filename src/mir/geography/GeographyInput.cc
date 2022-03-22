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


#include "mir/geography/GeographyInput.h"

#include "mir/util/Exceptions.h"


namespace mir {
namespace geography {


GeographyInput::GeographyInput(Geometry geometry) : geometry_(geometry) {
    geometryAsString(geometry_);  // just to assert
}


std::string GeographyInput::geometryAsString(Geometry geometry) {
    return geometry == Geometry::LonLat ? "LonLat" : geometry == Geometry::Spherical ? "Spherical" : NOTIMP;
}


GeographyInput::Geometry GeographyInput::stringAsGeometry(const std::string& geometry) {
    return geometry == "lonlat" ? Geometry::LonLat : geometry == "spherical" ? Geometry::Spherical : NOTIMP;
}


std::string GeographyInput::geometryAsString() const {
    return geometryAsString(geometry_);
}


}  // namespace geography
}  // namespace mir
