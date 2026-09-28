// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/spec/Exceptions.h"


namespace eckit::geo {


class Exception : public eckit::Exception {
public:

    using eckit::Exception::Exception;
};


}  // namespace eckit::geo


namespace eckit::geo::exception {


using NotImplemented = ::eckit::NotImplemented;
using SeriousBug     = ::eckit::SeriousBug;
using SpecError      = ::eckit::spec::exception::SpecError;


class AreaError : public geo::Exception {
public:

    explicit AreaError(const std::string&, const CodeLocation&);
};


class FigureError : public geo::Exception {
public:

    explicit FigureError(const std::string&, const CodeLocation&);
};


class GridError : public geo::Exception {
public:

    explicit GridError(const std::string&, const CodeLocation&);
};


class GridUnknownError : public geo::Exception {
public:

    explicit GridUnknownError(const std::string&, const CodeLocation&);
};


class OrderError : public geo::Exception {
public:

    explicit OrderError(const std::string&, const CodeLocation&);
};


class ProjectionError : public geo::Exception {
public:

    explicit ProjectionError(const std::string&, const CodeLocation&);
};


class RangeError : public geo::Exception {
public:

    explicit RangeError(const std::string&, const CodeLocation&);
};


class SearchError : public geo::Exception {
public:

    explicit SearchError(const std::string&, const CodeLocation&);
};


}  // namespace eckit::geo::exception
