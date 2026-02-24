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

#include "eckit/exception/Exceptions.h"
#include "eckit/spec/Exceptions.h"


namespace eckit::geo {


class Exception : public eckit::Exception {
public:

    using eckit::Exception::Exception;
};


}  // namespace eckit::geo


namespace eckit::geo::exception {


using SeriousBug = ::eckit::SeriousBug;
using SpecError  = ::eckit::spec::exception::SpecError;


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
