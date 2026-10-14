// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit_exceptions.h"

#include "AreaWrapper.h"
#include "FigureWrapper.h"

#include "eckit/geo/Exceptions.h"

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

double FigureWrapper::R() const {
    if (!figure_.spherical()) {
        throw eckit::geo::exception::FigureError("Figure: R is not defined for a non-spherical figure", Here());
    }
    return figure_.R();
}

double FigureWrapper::a() const {
    return figure_.a();
}

double FigureWrapper::b() const {
    return figure_.b();
}

double FigureWrapper::area() const {
    return figure_.area();
}

double FigureWrapper::area_within(const AreaWrapper& bbox) const {
    return figure_.area(bbox.inner_bbox());
}

rust::String FigureWrapper::proj_str() const {
    return rust::String(figure_.proj_str());
}

bool FigureWrapper::spherical() const {
    return figure_.spherical();
}

double FigureWrapper::eccentricity() const {
    return figure_.eccentricity();
}

double FigureWrapper::flattening() const {
    return figure_.flattening();
}

bool FigureWrapper::is_default() const {
    return figure_.is_default();
}

bool FigureWrapper::equals(const FigureWrapper& other) const {
    return figure_ == other.figure_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
