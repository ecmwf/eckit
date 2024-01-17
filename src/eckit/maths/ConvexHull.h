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

#include <vector>


namespace eckit::maths {


class ConvexHull {
public:
    ConvexHull(const ConvexHull&) = delete;
    ConvexHull(ConvexHull&&)      = delete;

    virtual ~ConvexHull() = default;

    void operator=(const ConvexHull&) = delete;
    void operator=(ConvexHull&&)      = delete;

    virtual std::vector<size_t> list_vertices() const                    = 0;
    virtual std::vector<std::vector<size_t>> list_facets() const         = 0;
    virtual std::vector<std::vector<size_t>> list_facets(size_t n) const = 0;

protected:
    ConvexHull() /*noexcept*/ = default;
};


}  // namespace eckit::maths
