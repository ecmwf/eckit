// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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
