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


#include <map>
#include <string>
#include <vector>


namespace orgQhull {
class Qhull;
}


namespace eckit::maths {


class Qhull {
public:

    // -- Types

    using coord_t    = std::vector<double>;
    using facets_n_t = std::map<size_t, size_t>;

    static const char* COMMAND_DEFAULT;

    // -- Constructors

    Qhull(size_t N, const coord_t& coord, const std::string& command = COMMAND_DEFAULT);

    Qhull(const Qhull&) = delete;
    Qhull(Qhull&&)      = delete;

    // -- Destructor

    ~Qhull();

    // -- Operators

    Qhull& operator=(const Qhull&) = delete;
    Qhull& operator=(Qhull&&)      = delete;

    // -- Methods

    std::vector<size_t> list_vertices() const;
    std::vector<std::vector<size_t>> list_facets() const;

    facets_n_t facets_n() const;
    std::vector<size_t> facets(size_t n) const;

private:

    // -- Members

    orgQhull::Qhull* qh_;
};


}  // namespace eckit::maths
