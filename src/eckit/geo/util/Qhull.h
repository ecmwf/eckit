/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once


#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/ConvexHull.h"

#include "libqhullcpp/Qhull.h"


namespace eckit::geo::util {


class Qhull : public ConvexHull {
public:
    // -- Types

    using coord_t = std::vector<double>;

    struct Exception : eckit::Exception {
        Exception(const std::string& what, int _errorCode, const std::string& _command) :
            eckit::Exception(what), errorCode(_errorCode), command(_command) {}
        const int errorCode;
        const std::string& command;
    };

    // -- Constructors

    Qhull(size_t N, const coord_t& coord, const std::string& command);

    // -- Overridden methods

    std::vector<size_t> list_vertices() const override;
    std::vector<std::vector<size_t>> list_facets() const override;
    std::vector<Triangle> list_triangles() const override;

private:
    // -- Members

    orgQhull::Qhull qh_;
};


}  // namespace eckit::geo::util
