// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::exception {


AreaError::AreaError(const std::string& what, const CodeLocation& location) :
    Exception("AreaError: [" + what + "]", location) {}


FigureError::FigureError(const std::string& what, const CodeLocation& location) :
    Exception("FigureError: [" + what + "]", location) {}


GridError::GridError(const std::string& what, const CodeLocation& location) :
    Exception("GridError: [" + what + "]", location) {}


GridUnknownError::GridUnknownError(const std::string& what, const CodeLocation& location) :
    Exception("GridUnknownError: [" + what + "]", location) {}


OrderError::OrderError(const std::string& what, const CodeLocation& location) :
    Exception("OrderError: [" + what + "]", location) {}


ProjectionError::ProjectionError(const std::string& what, const CodeLocation& location) :
    Exception("ProjectionError: [" + what + "]", location) {}


RangeError::RangeError(const std::string& what, const CodeLocation& location) :
    Exception("RangeError: [" + what + "]", location) {}


SearchError::SearchError(const std::string& what, const CodeLocation& location) :
    Exception("SearchError: [" + what + "]", location) {}


}  // namespace eckit::geo::exception
