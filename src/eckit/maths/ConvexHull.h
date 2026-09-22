// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/Qhull.h"


namespace eckit::maths {


class ConvexHull {
public:

    // -- Types

    using coord_t    = typename Qhull::coord_t;
    using facets_n_t = typename Qhull::facets_n_t;

    struct Exception : eckit::Exception {
        Exception(const std::string& what, int _errorCode, const std::string& _command) :
            eckit::Exception(what), errorCode(_errorCode), command(_command) {}
        const int errorCode;
        const std::string& command;
    };

    struct DimensionError : Exception {
        using Exception::Exception;
    };

    struct InputError : Exception {
        using Exception::Exception;
    };

    struct OptionError : Exception {
        using Exception::Exception;
    };

    struct PrecisionError : Exception {
        using Exception::Exception;
    };

    struct TopologyError : Exception {
        using Exception::Exception;
    };

    // -- Constructors

    ConvexHull(const ConvexHull&) = delete;
    ConvexHull(ConvexHull&&)      = delete;

    // -- Destructor

    virtual ~ConvexHull() = default;

    // -- Operators

    void operator=(const ConvexHull&) = delete;
    void operator=(ConvexHull&&)      = delete;

    // -- Methods

    virtual std::vector<size_t> list_vertices() const            = 0;
    virtual std::vector<std::vector<size_t>> list_facets() const = 0;

    virtual facets_n_t facets_n() const                = 0;
    virtual std::vector<size_t> facets(size_t n) const = 0;

protected:

    // -- Constructors

    ConvexHull() /*noexcept*/ = default;
};


}  // namespace eckit::maths
